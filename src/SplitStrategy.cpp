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
#include "SplitStrategy.h"

#include "Slice.h"
#include "Term.h"
#include "Ideal.h"

SplitStrategy::SplitStrategy() {
}

SplitStrategy::~SplitStrategy() {
}

// This common base class provides code that is useful for writing
// pivot split strategies.
class SplitStrategyCommon : public SplitStrategy {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	fputs
	  ("INTERNAL ERROR: Requested pivot split of non-pivot split strategy.\n",
	   stderr);
	ASSERT(false);
	exit(1);
  }

  virtual size_t getLabelSplitVariable(const Slice& slice) {
	fputs
	  ("INTERNAL ERROR: Requested label split of non-label split strategy.\n",
	   stderr);
	ASSERT(false);
	exit(1);
	return 0;
  }

  virtual bool isPivotSplit() {
	return false;
  }

  virtual bool isLabelSplit() {
	return false;
  }

  virtual bool isFrobeniusSplit() {
	return false;
  }

protected:
  static size_t getRandomSupportVar(const Term& term) {
	ASSERT(!term.isIdentity());

	size_t selected = rand() % term.getSizeOfSupport();
	for (size_t var = 0; ; ++var) {
	  ASSERT(var < term.getVarCount());
	  if (term[var] == 0)
		continue;

	  if (selected == 0)
		return var;
	  --selected;
	}
  }

  static Exponent getMedianPositiveExponentOf(Slice& slice, size_t var) {
	slice.singleDegreeSortIdeal(var);
	Ideal::const_iterator end = slice.getIdeal().end();
	Ideal::const_iterator begin = slice.getIdeal().begin();
	while ((*begin)[var] == 0) {
	  ++begin;
	  ASSERT(begin != end);
	}
	return (*(begin + (distance(begin, end) ) / 2))[var];
  }

  // Returns the variable that divides the most minimal generators of
  // those where some minimal generator is divisible by the square of
  // that variable.
  static size_t getBestVar(const Slice& slice) {
	Term co(slice.getVarCount());
	slice.getIdeal().getSupportCounts(co);

	const Term& lcm = slice.getLcm();
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (lcm[var] <= 1)
		co[var] = 0;

	ASSERT(!co.isIdentity());

	Exponent maxCount = co[co.getFirstMaxExponent()];
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (co[var] < maxCount)
		co[var] = 0;

	// Choose a deterministically random variable among those that are
	// best. This helps to avoid getting into a bad pattern.
	return getRandomSupportVar(co);
  }
};

class LabelSplit : public SplitStrategyCommon {
protected:
  Term _counts;
  void setCounts(const Slice& slice) {
	_counts.reset(slice.getVarCount());
	slice.getIdeal().getSupportCounts(_counts);	
  }

  Term _oneCounts;
  void setOneCounts(const Slice& slice) {
	// For each variable, count number of terms with exponent equal to 1,
	// not counting pure powers.
	_oneCounts.reset(slice.getVarCount());

	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it) {
	  if (getSizeOfSupport(*it, slice.getVarCount()) == 1)
		continue; // Not counting pure powers.
	  for (size_t var = 0; var < slice.getVarCount(); ++var)
		if ((*it)[var] == 1)
		  _oneCounts[var] += 1;
	}
  }

  virtual bool isLabelSplit() {
	return true;
  }
};

// Use the variable that divides the most minimal generators.
class MaxLabelSplit : public LabelSplit {
public:
  virtual size_t getLabelSplitVariable(const Slice& slice) {
	setCounts(slice);
	return _counts.getFirstMaxExponent();
  }
};

// Use the first variable that is valid for a label split.
class VarLabelSplit : public LabelSplit {
  virtual size_t getLabelSplitVariable(const Slice& slice) {
	setOneCounts(slice);

	for (size_t var = 0; ; ++var) {
		ASSERT(var < slice.getVarCount());
		if (_oneCounts[var] > 0)
		  return var;
	  }
	}
};

// Among those variables with least number of exponents equal to one,
// use the variable that divides the most minimal generators.
class MinLabelSplit : public LabelSplit {
  virtual size_t getLabelSplitVariable(const Slice& slice) {
	setCounts(slice);
	setOneCounts(slice);

	// Zero those variables of _counts that have more than the least number
	// of exponent 1 minimal generators.
	size_t mostGeneric = 0;
	for (size_t var = 1; var < slice.getVarCount(); ++var)
	  if (mostGeneric == 0 ||
		  (mostGeneric > _oneCounts[var] && _oneCounts[var] > 0))
		mostGeneric = _oneCounts[var];
	for (size_t var = 0; var < slice.getVarCount(); ++var)
	  if (_oneCounts[var] != mostGeneric)
		_counts[var] = 0;

	return _counts.getFirstMaxExponent();
  }
};

class PivotSplit : public SplitStrategyCommon {
public:
  virtual bool isPivotSplit() {
	return true;
  }
};

class MinimumSplit : public PivotSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	size_t var = getBestVar(slice);
	pivot.setToIdentity();
	pivot[var] = 1;
  }
};

class MedianSplit : public PivotSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	size_t var = getBestVar(slice);

	pivot.setToIdentity();
	pivot[var] = getMedianPositiveExponentOf(slice, var);
	if (pivot[var] == slice.getLcm()[var])
	  pivot[var] -= 1;
  }
};

class MaximumSplit : public PivotSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	size_t var = getBestVar(slice);
	pivot.setToIdentity();
	pivot[var] = slice.getLcm()[var] - 1;
  }
};

class IndependencePivotSplit : public MedianSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	if (slice.getVarCount() == 1) {
	  MedianSplit::getPivot(pivot, slice);
	  return;
	}

	for (int attempts = 0; attempts < 10; ++attempts) {
	  // Pick two distinct variables.
	  size_t var1 = rand() % slice.getVarCount();
	  size_t var2 = rand() % (slice.getVarCount() - 1);
	  if (var2 >= var1)
		++var2;

	  // Make pivot as big as it can be while making var1 and var2
	  // independent of each other.
	  bool first = true;
	  Ideal::const_iterator end = slice.getIdeal().end();
	  for (Ideal::const_iterator it = slice.getIdeal().begin();
		   it != end; ++it) {
		if ((*it)[var1] == 0 || (*it)[var2] == 0)
		  continue;

		if (first)
		  pivot = *it;
		else {
		  for (size_t var = 0; var < slice.getVarCount(); ++var)
			if (pivot[var] >= (*it)[var])
			  pivot[var] = (*it)[var] - 1;
		}
	  }

	  if (!first && !pivot.isIdentity())
		return;
	}

	MedianSplit::getPivot(pivot, slice);
  }
};

class GcdSplit : public PivotSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	size_t var = getBestVar(slice);

	size_t nonDivisibleCount = 0;
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it)
	  if ((*it)[var] >= 2)
		++nonDivisibleCount;

	for (int i = 0; i < 3; ++i) {
	  size_t selected = rand() % nonDivisibleCount;
	  for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
		ASSERT(it != end);
		if ((*it)[var] < 2)
		  continue;

		if (selected == 0) {
		  if (i == 0)
			pivot = *it;
		  else
			pivot.gcd(pivot, *it);
		  break;
		}
		--selected;
	  }
	}

	pivot.decrement();
  }
};

class MinGenSplit : public PivotSplit {
public:
  virtual void getPivot(Term& pivot, Slice& slice) {
	size_t nonSquareFreeCount = 0;
	Ideal::const_iterator end = slice.getIdeal().end();
	for (Ideal::const_iterator it = slice.getIdeal().begin();
		 it != end; ++it)
	  if (!::isSquareFree(*it, slice.getVarCount()))
		++nonSquareFreeCount;
	
	size_t selected = rand() % nonSquareFreeCount;
	for (Ideal::const_iterator it = slice.getIdeal().begin(); ; ++it) {
	  ASSERT(it != end);
	  if (::isSquareFree(*it, slice.getVarCount()))
		continue;
	  
	  if (selected == 0) {
		pivot = *it;
		break;
	  }
	  --selected;
	}
	
	pivot.decrement();
  }
};

class FrobeniusSplit : public PivotSplit {
public:
  virtual bool isFrobeniusSplit() {
	return true;
  }

  virtual void getPivot(Term& pivot, Slice& slice) {
	fputs
	  ("INTERNAL ERROR: Called getPivot directly on FrobeniusSplit.\n",
	   stderr);
	ASSERT(false);
	exit(1);
  }
};

SplitStrategy* SplitStrategy::getStrategy(const string& name) {
  if (name == "maxlabel")
	return new MaxLabelSplit();
  if (name == "minlabel")
	return new MinLabelSplit();
  if (name == "varlabel")
	return new VarLabelSplit();

  if (name == "minimum")
	return new MinimumSplit();
  if (name == "median")
	return new MedianSplit();
  if (name == "maximum")
	return new MaximumSplit();
  if (name == "mingen")
	return new MinGenSplit();
  if (name == "indep")
	return new IndependencePivotSplit();
  if (name == "gcd")
	return new GcdSplit();

  if (name == "frob")
	return new FrobeniusSplit();

  return 0;
}
