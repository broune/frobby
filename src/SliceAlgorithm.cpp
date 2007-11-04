#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "TermList.h"
#include "Term.h"
#include "VarNames.h"
#include "io.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "TermList.h"
#include "Slice.h"
#include "DecomConsumer.h"
#include "DecomWriter.h"
#include "SliceStrategy.h"

#include "IndependenceSplitter.h"

SliceAlgorithm::SliceAlgorithm() {
}

void SliceAlgorithm::setConsumer(DecomConsumer* consumer) {
  _decomConsumer = consumer;
}

void SliceAlgorithm::setStrategy(SliceStrategy* strategy) {
  _strategy = strategy;
}

void SliceAlgorithm::runAndDeleteIdealAndReset(Ideal* ideal) {
  ASSERT(_decomConsumer != 0);
  ASSERT(_strategy != 0);

  if (!ideal->isZeroIdeal()) {
    Slice slice(ideal->clone(),
		new TermList(ideal->getVariableCount()),
		Term(ideal->getVariableCount()));
    content(slice);
  }

  delete _decomConsumer;
  _decomConsumer = 0;

  delete _strategy;
  _strategy = 0;

  delete ideal;
}

// Requires slice.getIdeal() to be minimized.
bool SliceAlgorithm::independenceSplit(Slice& slice) {
  static Partition partition;

  IndependenceSplitter::computePartition(partition, slice);
  if (IndependenceSplitter::shouldPerformSplit(partition, slice))
    return false;

  IndependenceSplitter indep(partition, slice);

  DecomConsumer* oldConsumer = _decomConsumer;
  _decomConsumer = &indep;

  Slice projSlice;
  for (size_t i = 0; i < indep.getChildCount(); ++i) {
    indep.setCurrentChild(i, projSlice);
    content(projSlice, true);
  }
  _decomConsumer = oldConsumer;

  indep.generateDecom(_decomConsumer);

  return true;
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

  {
    Slice inner(slice.getIdeal()->createMinimizedColon(pivot),
		slice.getSubtract()->createMinimizedColon(pivot),
		slice.getMultiply(), pivot);
    inner.normalize();
    content(inner);
  }

  {
    Slice outer(slice.getIdeal()->clone(),
		slice.getSubtract()->clone(),
		slice.getMultiply());

    outer.getIdeal()->removeStrictMultiples(pivot);
    if (pivot.getSizeOfSupport() > 1)
      outer.getSubtract()->insert(pivot);

    content(outer);
  }
}

void SliceAlgorithm::content(Slice& slice, bool simplifiedAndDependent) {
  if (!simplifiedAndDependent)
    slice.simplify();

  _strategy->startingContent(slice);

  if (!slice.baseCase(_decomConsumer)) {
    if (simplifiedAndDependent || !independenceSplit(slice)) {
      switch (_strategy->getSplitType(slice)) {
      case SliceStrategy::LabelSplit:
	labelSplit(slice);
	break;

      case SliceStrategy::PivotSplit:
	pivotSplit(slice);
	break;
      }
    }
  }

  _strategy->endingContent(slice);
}
