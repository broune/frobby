#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "Term.h"
#include "VarNames.h"
#include "io.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "Slice.h"
#include "SliceStrategy.h"

#include "IndependenceSplitter.h"

SliceAlgorithm::SliceAlgorithm():
  _useIndependence(true),
  _strategy(0) {
}

void SliceAlgorithm::setStrategy(SliceStrategy* strategy) {
  delete _strategy;
  _strategy = strategy;
}

void SliceAlgorithm::setUseIndependence(bool useIndependence) {
  _useIndependence = useIndependence;
}

void SliceAlgorithm::runAndClear(Ideal& ideal) {
  ASSERT(_strategy != 0);

  if (ideal.getGeneratorCount() > 0) {
    Term initialMultiply(ideal.getVarCount());
    for (size_t var = 0; var < initialMultiply.getVarCount(); ++var)
      initialMultiply[var] = 1;

    Slice slice(ideal, Ideal(ideal.getVarCount()), initialMultiply);
    _strategy->initialize(slice);
    content(slice);
  }

  ideal.clear();

  // Now reset the fields to their default values.
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

  _strategy->doingIndependenceSplit(slice, indep.getMixedProjectionSubtract());

  slice.clear(); // to save memory

  for (size_t i = 0; i < indep.getChildCount(); ++i) {
    Slice& projSlice = indep.getSlice(i);

    bool last = (i == indep.getChildCount() - 1);
    _strategy->doingIndependentPart(indep.getProjection(i), last);

    content(projSlice, true);

    if (!_strategy->doneWithIndependentPart())
      break;
  }

  _strategy->doneWithIndependenceSplit();

  return true;
}

void SliceAlgorithm::labelSplit2(Slice& slice) {
  size_t var = _strategy->getLabelSplitVariable(slice);

  Ideal* ideal = new Ideal(slice.getVarCount());
  
  Ideal::const_iterator stop = slice.getIdeal().end();
  for (Ideal::const_iterator it = slice.getIdeal().begin();
       it != stop; ++it)
    if ((*it)[var] == 0)
      ideal->insert(*it);

  Term pivot(slice.getVarCount());
  pivot[var] = 1;
  ideal->insert(pivot);
  
  size_t childCount = 0;
  for (Ideal::const_iterator it = slice.getIdeal().begin();
       it != stop; ++it) {
    if ((*it)[var] != 1)
      continue;
    if (childCount > 0)
      slice.getSubtract().insert(pivot); // This is the previous pivot

    pivot = *it;
    pivot[var] -= 1;

    {
      Slice child(*ideal, slice.getSubtract(), slice.getMultiply());
      child.innerSlice(pivot);
      content(child);
    }
    ++childCount;
  }

  delete ideal;

  pivot.setToIdentity();
  pivot[var] = 1;
  slice.innerSlice(pivot);

  content(slice);
}

void SliceAlgorithm::labelSplit(Slice& slice) {
  size_t var = _strategy->getLabelSplitVariable(slice);
  slice.singleDegreeSortIdeal(var);

  Ideal* cumulativeSubtract = &(slice.getSubtract());
  Term labelMultiply(slice.getVarCount());
  Term pivot(slice.getVarCount());

  for (Ideal::const_iterator it = slice.getIdeal().begin();
       it != slice.getIdeal().end(); ++it) {
    if ((*it)[var] == 0)
      continue;

    pivot = *it;
    pivot[var] -= 1;

    {
      Slice child(slice.getIdeal(), *cumulativeSubtract, slice.getMultiply());
      child.innerSlice(pivot);
      content(child);
    }

    Ideal::const_iterator next = it;
    ++next;
    if (next != slice.getIdeal().end() && (*it)[var] == (*next)[var]) {
      if (cumulativeSubtract == &(slice.getSubtract()))
	cumulativeSubtract = new Ideal(slice.getSubtract());
      cumulativeSubtract->insert(pivot);
    } else if (cumulativeSubtract != &(slice.getSubtract())) {
      delete cumulativeSubtract;
      cumulativeSubtract = &(slice.getSubtract());
    }
  }
}

void SliceAlgorithm::pivotSplit(Slice& slice) {
  // These scopes are made to preserve memory resources.
  {
    Slice inner(slice);

    {
      Term pivot(slice.getVarCount());
      _strategy->getPivot(pivot, slice);

      ASSERT(!pivot.isIdentity()); 
      ASSERT(!slice.getIdeal().contains(pivot));
      ASSERT(!slice.getSubtract().contains(pivot));

      inner.innerSlice(pivot);
      slice.outerSlice(pivot);
    }

    content(inner);
  }

  // Handle outer slice.
  content(slice);
}

void SliceAlgorithm::content(Slice& slice, bool simplifiedAndDependent) {
  _strategy->startingContent(slice);
  if (!simplifiedAndDependent)
    _strategy->simplify(slice);

  if (!slice.baseCase(_strategy) &&
      (simplifiedAndDependent || !independenceSplit(slice))) {
    switch (_strategy->getSplitType(slice)) {
    case SliceStrategy::LabelSplit:
      labelSplit(slice);
      break;

    case SliceStrategy::PivotSplit:
      pivotSplit(slice);
      break;

    default:
      ASSERT(false);
    }
  }

  _strategy->endingContent();
}

bool computeSingleMSM2(const Slice& slice, Term& msm) {
  msm.reset(slice.getVarCount());
  Term lcm(slice.getLcm());

  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] = lcm[var];
    Ideal::const_iterator it = slice.getIdeal().begin();
    Ideal::const_iterator end = slice.getIdeal().end();
    for (; it != end; ++it) {
      if (msm.dominates(*it)) {
	ASSERT((*it)[var] > 0);
	msm[var] = (*it)[var] - 1;
      }
    }
  }

#ifdef DEBUG
  ASSERT(!slice.getIdeal().contains(msm));
  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] += 1;
    ASSERT(slice.getIdeal().contains(msm));
    msm[var] -= 1;
  }
#endif

  if (slice.getSubtract().contains(msm))
    return false;

  msm.product(msm, slice.getMultiply());
  return true;
}

bool computeSingleMSM(const Slice& slice, Term& msm) {
  for (size_t var = 0; var < msm.getVarCount(); ++var)
    if (slice.getLcm()[0] == 0)
      return false;

  // Extract terms into a container we can alter without changing
  // slice.
  vector<const Exponent*> terms;
  {
    terms.reserve(slice.getIdeal().getGeneratorCount());
    Ideal::const_iterator it = slice.getIdeal().begin();
    Ideal::const_iterator end = slice.getIdeal().end();
    for (; it != end; ++it)
      terms.push_back(*it);
  }

  msm.reset(slice.getVarCount());
  msm[0] = slice.getLcm()[0] - 1; // do offset 0 to avoid negative vars below
  {
    // Do things from the right so that the call do dominates below
    // will exit earlier.
    for (size_t var = msm.getVarCount() - 1; var >= 1; --var) {
      msm[var] = slice.getLcm()[var];

      bool foundLabel = false;
      vector<const Exponent*>::iterator it = terms.begin();
      vector<const Exponent*>::iterator end = terms.end();
      for (; it != end; ++it) {
	if (msm.dominates(*it)) {
	  ASSERT((*it)[var] > 0);
	  msm[var] = (*it)[var] - 1;
	  foundLabel = true;
	}
      }
      if (!foundLabel)
	return false;

      // Remove irrelevant terms
      it = terms.begin();
      for (; it != terms.end();) {
	if ((*it)[var] <= msm[var])
	  ++it;
	else {
	  if (it + 1 != terms.end())
	    *it = terms.back();
	  terms.pop_back();
	}
      }
    }
  }

#ifdef DEBUG
  ASSERT(!slice.getIdeal().contains(msm));
  for (size_t var = 0; var < msm.getVarCount(); ++var) {
    msm[var] += 1;
    ASSERT(slice.getIdeal().contains(msm));
    msm[var] -= 1;
  }
#endif

  if (slice.getSubtract().contains(msm))
    return false;

  msm.product(msm, slice.getMultiply());
  return true;
}
