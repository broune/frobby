#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "Term.h"
#include "VarNames.h"
#include "io.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "Slice.h"
#include "DecomConsumer.h"
#include "DecomWriter.h"
#include "SliceStrategy.h"

#include "IndependenceSplitter.h"

SliceAlgorithm::SliceAlgorithm():
  _useIndependence(true),
  _decomConsumer(0),
  _strategy(0) {
}

void SliceAlgorithm::setConsumer(DecomConsumer* consumer) {
  delete _decomConsumer;
  _decomConsumer = consumer;
}

void SliceAlgorithm::setStrategy(SliceStrategy* strategy) {
  delete _strategy;
  _strategy = strategy;
}

void SliceAlgorithm::setUseIndependence(bool useIndependence) {
  _useIndependence = useIndependence;
}

void SliceAlgorithm::runAndDeleteIdealAndReset(Ideal* ideal) {
  ASSERT(_decomConsumer != 0);
  ASSERT(_strategy != 0);

  if (!ideal->isZeroIdeal()) {
    Slice slice(ideal->clone(),
		new Ideal(ideal->getVarCount()),
		Term(ideal->getVarCount()));
    content(slice);
  }

  delete ideal;

  // Now reset the fields to their default values.
  delete _decomConsumer;
  _decomConsumer = 0;

  delete _strategy;
  _strategy = 0;

  _useIndependence = true;
}

// Requires slice.getIdeal() to be minimized.
bool SliceAlgorithm::independenceSplit(Slice& slice) {
  if (!_useIndependence)
    return false;

  static Partition partition;

  IndependenceSplitter::computePartition(partition, slice);
  if (!IndependenceSplitter::shouldPerformSplit(partition, slice))
    return false;

  IndependenceSplitter indep(partition, slice);

  DecomConsumer* oldConsumer = _decomConsumer;
  _decomConsumer = &indep;

  bool decomMustBeEmpty = false;

  Slice projSlice;
  for (size_t i = 0; i < indep.getChildCount(); ++i) {
    indep.setCurrentChild(i, projSlice);
    content(projSlice, true);
    if (indep.currentChildDecomIsEmpty()) {
      decomMustBeEmpty = true;
      break;
    }
  }
  _decomConsumer = oldConsumer;

  if (!decomMustBeEmpty)
    indep.generateDecom(_decomConsumer);

  return true;
}

void SliceAlgorithm::labelSplit2(Slice& slice) {
  size_t var = _strategy->getLabelSplitVariable(slice);

  Ideal* ideal = slice.getIdeal()->createNew(slice.getVarCount());
  
  Ideal::const_iterator stop = slice.getIdeal()->end();
  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != stop; ++it)
    if ((*it)[var] == 0)
      ideal->insert(*it);

  Term pivot(slice.getVarCount());
  pivot[var] = 1;
  ideal->insert(pivot);
  
  size_t childCount = 0;
  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != stop; ++it) {
    if ((*it)[var] != 1)
      continue;
    if (childCount > 0)
      slice.getSubtract()->insert(pivot); // This is the previous pivot

    pivot = *it;
    pivot[var] -= 1;

    {
      Slice child(ideal->createMinimizedColon(pivot),
		  slice.getSubtract()->createMinimizedColon(pivot),
		  slice.getMultiply(), pivot);

      child.normalize();
      content(child);
    }
    ++childCount;
  }

  delete ideal;

  pivot.setToIdentity();
  pivot[var] = 1;

  slice.getIdeal()->colonReminimize(pivot);
  slice.getSubtract()->colonReminimize(pivot);
  slice.getMultiply()[var] += 1;
  slice.normalize();


  content(slice);
}

void SliceAlgorithm::labelSplit(Slice& slice) {
  size_t var = _strategy->getLabelSplitVariable(slice);
  slice.getIdeal()->singleDegreeSort(var);

  Ideal* cumulativeSubtract = slice.getSubtract();
  Term labelMultiply(slice.getVarCount());
  Term pivot(slice.getVarCount());

  for (Ideal::const_iterator it = slice.getIdeal()->begin();
       it != slice.getIdeal()->end(); ++it) {
    if ((*it)[var] == 0)
      continue;

    pivot = *it;
    pivot[var] -= 1;

    {
      Slice child(slice.getIdeal()->createMinimizedColon(pivot),
		  cumulativeSubtract->createMinimizedColon(pivot),
		  slice.getMultiply(), pivot);
      child.normalize();
      content(child);
    }

    Ideal::const_iterator next = it;
    ++next;
    if (next != slice.getIdeal()->end() && (*it)[var] == (*next)[var]) {
      if (cumulativeSubtract == slice.getSubtract())
	cumulativeSubtract = slice.getSubtract()->clone();
      cumulativeSubtract->insert(pivot);
    } else if (cumulativeSubtract != slice.getSubtract()) {
      delete cumulativeSubtract;
      cumulativeSubtract = slice.getSubtract();
    }
  }
}

void SliceAlgorithm::pivotSplit(Slice& slice) {
  Term pivot(slice.getVarCount());
  _strategy->getPivot(pivot, slice);

  // Handle inner slice.
  {
    Slice inner(slice.getIdeal()->createMinimizedColon(pivot),
		slice.getSubtract()->createMinimizedColon(pivot),
		slice.getMultiply(), pivot);
    inner.normalize();
    content(inner);
  }

  // Handle outer slice.
  slice.getIdeal()->removeStrictMultiples(pivot);
  if (pivot.getSizeOfSupport() > 1)
    slice.getSubtract()->insert(pivot);
  
  content(slice);
}

void SliceAlgorithm::content(Slice& slice, bool simplifiedAndDependent) {
  if (!simplifiedAndDependent)
    slice.simplify();

  _strategy->startingContent(slice);

  if (!slice.baseCase(_decomConsumer)) {
    if (simplifiedAndDependent || !independenceSplit(slice)) {
      switch (_strategy->getSplitType(slice)) {
      case SliceStrategy::LabelSplit:
	labelSplit2(slice);
	break;

      case SliceStrategy::PivotSplit:
	pivotSplit(slice);
	break;
      }
    }
  }

  _strategy->endingContent();
}
