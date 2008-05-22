/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "Term.h"
#include "VarNames.h"
#include "IOHandler.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "Partition.h"
#include "MsmSlice.h"
#include "MsmStrategy.h"

#include "IndependenceSplitter.h"

SliceAlgorithm::SliceAlgorithm():
  _useIndependence(true),
  _strategy(0) {
}

void SliceAlgorithm::setStrategy(MsmStrategy* strategy) {
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

    MsmSlice slice(ideal, Ideal(ideal.getVarCount()), initialMultiply);
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
bool SliceAlgorithm::independenceSplit(MsmSlice& slice) {
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
    MsmSlice& projSlice = indep.getSlice(i);

    bool last = (i == indep.getChildCount() - 1);
    _strategy->doingIndependentPart(indep.getProjection(i), last);

    content(projSlice, true);

    if (!_strategy->doneWithIndependentPart())
      break;
  }

  _strategy->doneWithIndependenceSplit();

  return true;
}

pair<MsmSlice*, MsmSlice*> SliceAlgorithm::labelSplit(MsmSlice* slice) {
  ASSERT(!slice->normalize());
  size_t var = _strategy->getLabelSplitVariable(*slice);

  Term term(slice->getVarCount());

  const Term& lcm = slice->getLcm();

  Ideal::const_iterator stop = slice->getIdeal().end();
  Ideal::const_iterator label = stop;
  bool hasTwoLabels = false;
  for (Ideal::const_iterator it = slice->getIdeal().begin(); it != stop; ++it) {
    if ((*it)[var] == 1) {
	  term = *it;
	  term[var] -= 1;

	  bool couldBeLabel = !slice->getSubtract().contains(term);
	  if (couldBeLabel) {
		for (size_t v = 0; v < slice->getVarCount(); ++v) {
		  if (term[v] == lcm[v]) {
			couldBeLabel = false;
			break;
		  }
		}
	  }

	  if (couldBeLabel) {
		if (label == stop)
		  label = it;
		else {
		  hasTwoLabels = true;
		  break;
		}
	  }
	}
  }

  MsmSlice* hasLabelSlice = 0;
  MsmSlice* notLabelSlice = new MsmSlice(*slice);

  if (label != stop) {
	term = *label;
	term[var] -= 1;

	hasLabelSlice = new MsmSlice(*slice);
	hasLabelSlice->innerSlice(term);

	if (hasTwoLabels)
	  notLabelSlice->outerSlice(term);
  }

  if (!hasTwoLabels) {
	term.setToIdentity();
	term[var] = 1;
	notLabelSlice->innerSlice(term);
  }

  return make_pair(hasLabelSlice, notLabelSlice);
}

pair<MsmSlice*, MsmSlice*> SliceAlgorithm::pivotSplit(MsmSlice* slice) {
  Term pivot(slice->getVarCount());
  _strategy->getPivot(pivot, *slice);

  MsmSlice* inner = new MsmSlice(*slice);
  MsmSlice* outer = new MsmSlice(*slice);

  ASSERT(!pivot.isIdentity()); 
  ASSERT(!slice->getIdeal().contains(pivot));
  ASSERT(!slice->getSubtract().contains(pivot));

  inner->innerSlice(pivot);
  outer->outerSlice(pivot);

  return make_pair(inner, outer);
}

void SliceAlgorithm::content(MsmSlice& slice, bool simplifiedAndDependent) {
  _strategy->startingContent(slice);

  if (!slice.baseCase(_strategy) && !independenceSplit(slice)) {
	pair<MsmSlice*, MsmSlice*> slicePair = _strategy->split(&slice);
	if (slicePair.first != 0) {
	  content(*slicePair.first);
	  delete slicePair.first;
	}
	
	if (slicePair.second != 0) {
	  content(*slicePair.second);
	  delete slicePair.second;
	}
  }

  _strategy->endingContent();
}

bool computeSingleMSM2(const MsmSlice& slice, Term& msm) {
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

bool computeSingleMSM(const MsmSlice& slice, Term& msm) {
  for (size_t var = 0; var < msm.getVarCount(); ++var)
    if (slice.getLcm()[var] == 0)
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

  // do offset 0 to avoid negative vars below
  msm[0] = slice.getLcm()[0] - 1;
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
