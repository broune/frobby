/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#include "PivotStrategy.h"

#include "EulerState.h"
#include "NameFactory.h"
#include "RawSquareFreeTerm.h"
#include "ElementDeleter.h"
#include "PivotEulerAlg.h"

#include <sstream>
#include <limits>

namespace Ops = SquareFreeTermOps;

namespace {
  inline size_t getPopVar(const vector<size_t>& divCounts) {
	return max_element(divCounts.begin(), divCounts.end()) - divCounts.begin();
  }

  inline size_t getRareVar(const vector<size_t>& divCounts) {
	vector<size_t>::const_iterator end = divCounts.end();
	vector<size_t>::const_iterator rare = divCounts.begin();
	for (; rare != end; ++rare)
	  if (*rare > 0)
		break;

	vector<size_t>::const_iterator it = rare + 1;
	for (; it != end; ++it)
	  if (*it > 0 && *it < *rare)
		rare = it;

	return rare - divCounts.begin();
  }

  class RawSquareFreeTerm {
  public:
	RawSquareFreeTerm(): _term(0), _capacity(0) {}
	~RawSquareFreeTerm() {delete _term;}

	operator Word*() {return _term;}
	operator const Word*() const {return _term;}

	void reserve(size_t varCount) {
	  if (varCount > _capacity) {
		Ops::deleteTerm(_term);
		_term = Ops::newTerm(varCount);
		_capacity = varCount;
	  }
	}

  private:
	Word* _term;
	size_t _capacity;
  };

  class WithPivotTerm : public PivotStrategy {
  protected:
	Word* termWithCapacity(size_t varCount) {
	  _term.reserve(varCount);
	  return _term;
	}

  private:
	RawSquareFreeTerm _term;
  };

  class StdStrategy : public WithPivotTerm {
  public:
	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) = 0;
	virtual void computationCompleted(const PivotEulerAlg& alg) {}
  };

  class StdPopVar : public StdStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceStdSplit(getPopVar(divCounts), newState);
	}

	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  Word* pivot = termWithCapacity(varCount);
	  Ops::setToIdentity(pivot, varCount);
	  Ops::setExponent(pivot, getPopVar(divCounts), 1);
	  return pivot;
	}

	static const char* staticGetName() {
	  return "popvar";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class StdRareVar : public StdStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceStdSplit(getRareVar(divCounts), newState);
	}

	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  Word* pivot = termWithCapacity(varCount);
	  Ops::setToIdentity(pivot, varCount);
	  Ops::setExponent(pivot, getRareVar(divCounts), 1);
	  return pivot;
	}

	static const char* staticGetName() {
	  return "rarevar";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class StdPopGcd : public StdStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceStdSplit(getPivot(state, divCounts), newState);
	}

	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  const size_t popVar = getPopVar(divCounts);
	  Word* pivot = termWithCapacity(varCount);

	  if (divCounts[popVar] == 1) {
		Ops::setToIdentity(pivot, varCount);
		Ops::setExponent(pivot, getPopVar(divCounts), 1);
		return pivot;
	  }

	  size_t seen = 0;
	  RawSquareFreeIdeal::const_iterator it = state.getIdeal().begin();
	  RawSquareFreeIdeal::const_iterator end = state.getIdeal().end();
	  for (; it != end; ++it) {
		if (Ops::getExponent(*it, popVar) != 0) {
		  if (seen == 0)
			Ops::assign(pivot, *it, varCount);
		  else
			Ops::gcdInPlace(pivot, *it, varCount);
		  ++seen;
		  if (seen == 3)
			break;
		}
	  }
	  ASSERT(seen > 1);
	  return pivot;
	}

	static const char* staticGetName() {
	  return "popgcd";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class StdRandom  : public StdStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  const size_t random = getRandomNotEliminatedVar(state);
	  state.inPlaceStdSplit(random, newState);
	}

	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  const size_t random = getRandomNotEliminatedVar(state);
	  Word* pivot = termWithCapacity(varCount);
	  Ops::setToIdentity(pivot, varCount);
	  Ops::setExponent(pivot, random, 1);
	  return pivot;
	}

	static const char* staticGetName() {
	  return "random";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}

  private:
	size_t getRandomNotEliminatedVar(const EulerState& state) {
	  while (true) {
		size_t random = rand() % state.getVarCount();
		if (Ops::getExponent(state.getEliminatedVars(), random) == 0)
		  return random;
	  }
	}
  };

  class StdAny  : public StdStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  const size_t any = getAnyNotEliminatedVar(state);
	  state.inPlaceStdSplit(any, newState);
	}

	virtual Word* getPivot(const EulerState& state,
						   const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  const size_t any = getAnyNotEliminatedVar(state);
	  Word* pivot = termWithCapacity(varCount);
	  Ops::setToIdentity(pivot, varCount);
	  Ops::setExponent(pivot, any, 1);
	  return pivot;
	}

	static const char* staticGetName() {
	  return "any";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}

  private:
	size_t getAnyNotEliminatedVar(const EulerState& state) {
	  for (size_t var = 0; ; ++var) {
		ASSERT(var < state.getVarCount());
		if (Ops::getExponent(state.getEliminatedVars(), var) == 0)
		  return var;
	  }
	}
  };

  class StdWiden  : public WithPivotTerm {
  public:
	StdWiden(auto_ptr<StdStrategy> strat):
	  _strat(strat) {
	  ASSERT(_strat.get() != 0);
	}

	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  const size_t varCount = state.getVarCount();
	  Word* narrow = _strat->getPivot(state, divCounts);
	  Word* wide = termWithCapacity(varCount);
	  state.getIdeal().getGcdOfMultiples(wide, narrow);
	  state.inPlaceStdSplit(wide, newState);
	}

	virtual void getName(ostream& out) const {
	  out << "widen_";
	  _strat->getName(out);
	}

	virtual void computationCompleted(const PivotEulerAlg& alg) {
	  _strat->computationCompleted(alg);
	}

  private:
	auto_ptr<StdStrategy> _strat;
  };

  class GenStrategy : public WithPivotTerm {
  public:
	typedef RawSquareFreeIdeal::iterator iterator;

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) = 0;
	virtual void computationCompleted(const PivotEulerAlg& alg) {}
  };

  class GenPopVar : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  size_t pivotIndex = state.getIdeal().getMultiple(getPopVar(divCounts));
	  state.inPlaceGenSplit(pivotIndex, newState);
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  size_t popVar = getPopVar(divCounts);
	  const size_t varCount = divCounts.size();
	  Word* term = termWithCapacity(varCount);
	  Ops::setToIdentity(term, varCount);
	  for (size_t var = 0; var < varCount; ++var)
		if (divCounts[var] == divCounts[popVar])
		  Ops::setExponent(term, var, 1);

	  iterator newEnd = begin;
	  for (iterator it = begin; it != end; ++it) {
		if (Ops::isRelativelyPrime(term, *it, varCount))
		  continue;
		Ops::swap(*it, *newEnd, varCount);
		++newEnd;
	  }
	  return newEnd;
	}

	static const char* staticGetName() {
	  return "popvar";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class GenRareVar : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  size_t pivotIndex = state.getIdeal().getMultiple(getRareVar(divCounts));
	  state.inPlaceGenSplit(pivotIndex, newState);
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  size_t rareVar = getRareVar(divCounts);
	  const size_t varCount = divCounts.size();

	  iterator newEnd = begin;
	  for (iterator it = begin; it != end; ++it) {
		if (Ops::getExponent(*it, rareVar) == 0)
		  continue;
		Ops::swap(*it, *newEnd, varCount);
		++newEnd;
	  }
	  return newEnd;
	}

	static const char* staticGetName() {
	  return "rarevar";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class GenComposite : public PivotStrategy {
  public:
	GenComposite():
	  _filters(0),
	  _filtersDeleter(_filters) {
	}

	typedef RawSquareFreeIdeal::iterator iterator;

	void addStrategy(auto_ptr<GenStrategy> strat) {
	  exceptionSafePushBack(_filters, strat);
	}

	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  const iterator begin = state.getIdeal().begin();
	  iterator end = state.getIdeal().end();
	  ASSERT(end - begin > 0);

	  for (size_t i = 0; i < _filters.size(); ++i)
		end = _filters[i]->filter(begin, end, divCounts);
	  ASSERT(end - begin > 0);

	  state.inPlaceGenSplit(0, newState);
	}

	virtual void getName(ostream& out) const {
	  for (size_t i = 0; i < _filters.size(); ++i) {
		if (i > 0)
		  out << '_';
		_filters[i]->getName(out);
	  }
	}

	virtual void computationCompleted(const PivotEulerAlg& alg) {
	  for (size_t i = 0; i < _filters.size(); ++i)
		_filters[i]->computationCompleted(alg);
	}

  private:
	vector<GenStrategy*> _filters;
	ElementDeleter<vector<GenStrategy*> > _filtersDeleter;
  };

  class GenRarestVars : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  filter(state.getIdeal().begin(), state.getIdeal().end(), divCounts);
	  state.inPlaceGenSplit(0, newState);
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  const size_t varCount = divCounts.size();
	  size_t lastDivCount = 0;
	  while (end - begin > 1) {
		size_t minDivCount = numeric_limits<size_t>::max();
		for (size_t var = 0; var < varCount; ++var)
		  if (divCounts[var] > lastDivCount && minDivCount > divCounts[var])
			minDivCount = divCounts[var];
		if (minDivCount == numeric_limits<size_t>::max())
		  break;

		end = filter(begin, end, divCounts, minDivCount);
		lastDivCount = minDivCount;
	  }
	  return end;
	}

	static const char* staticGetName() {
	  return "rarestvars";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}

  private:
	iterator filter(iterator begin, iterator end,
					const vector<size_t>& divCounts,
					size_t divCount) {
	  // Set the support of term to be the vars of the specified rarity
	  const size_t varCount = divCounts.size();
	  Word* term = termWithCapacity(varCount);
	  Ops::setToIdentity(term, varCount);
	  for (size_t var = 0; var < varCount; ++var)
		if (divCounts[var] == divCount)
		  Ops::setExponent(term, var, 1);

	  // Select the generators that are divisible by the most vars with
	  // the specified rarity.
	  iterator newEnd = begin;
	  size_t maxRareCount = 0;
	  _tmp.reserve(varCount);
	  Word* tmp = _tmp;
	  for (iterator it = begin; it != end; ++it) {
		if (Ops::isRelativelyPrime(term, *it, varCount))
		  continue; // no rare vars in *it

		Ops::gcd(tmp, term, *it, varCount);
		const size_t rareCount = Ops::getSizeOfSupport(tmp, varCount);

		if (maxRareCount > rareCount)
		  continue;
		if (maxRareCount < rareCount) {
		  maxRareCount = rareCount;
		  newEnd = begin;
		}
		Ops::swap(*newEnd, *it, varCount);
		++newEnd;
	  }
	  if (newEnd != begin)
		return newEnd;
	  else
		return end; // no rare vars in any generator, so we can't discard any
	}

	size_t getRarest(const RawSquareFreeIdeal& ideal,
					 const vector<size_t>& divCounts) {
	  RawSquareFreeIdeal::const_iterator it = ideal.begin();
	  const RawSquareFreeIdeal::const_iterator stop = ideal.end();
	  RawSquareFreeIdeal::const_iterator rarest = it;
  
	  for (; it != stop; ++it)
		if (rarer(*it, *rarest, divCounts))
		  rarest = it;
	  return rarest - ideal.begin();
	}

	/** return (a,b) where a is the minimum divCount > above in the
		support of term and b is the number of times that that divCount
		appears in the support. */
	pair<size_t, size_t> getRarity(const Word* const term,
								   const vector<size_t>& divCounts,
								   size_t above) {
	  size_t varCount = divCounts.size();
	  size_t rarity = varCount;
	  size_t multiplicity = 0;
	  for (size_t var = 0; var < varCount; ++var) {
		const size_t co = divCounts[var];
		if (Ops::getExponent(term, var) != 0 && co <= rarity && co > above) {
		  ASSERT(divCounts[var] != 0);
		  if (co == rarity)
			++multiplicity;
		  else {
			rarity = divCounts[var];
			multiplicity = 1;
		  }
		}
	  }
	  return make_pair(rarity, multiplicity);
	}

	bool rarer(const Word* const a, const Word* const b,
			   const vector<size_t>& divCounts) {	
	  size_t lookAbove = 0;
	  while (true) {
		pair<size_t, size_t> rarityA = getRarity(a, divCounts, lookAbove);
		pair<size_t, size_t> rarityB = getRarity(b, divCounts, lookAbove);

		if (rarityA.first < rarityB.first)
		  return true;
		if (rarityA.first > rarityB.first)
		  return false;

		if (rarityA.second > rarityB.second)
		  return true;
		if (rarityA.second < rarityB.second)
		  return false;

		if (rarityA.second == 0)
		  return false; // a and b are equally rare

		lookAbove = rarityA.first;
	  }
	}

	RawSquareFreeTerm _tmp;
  };

  class GenMaxSupport : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceGenSplit(state.getIdeal().getMaxSupportGen(), newState);	
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  const size_t varCount = divCounts.size();
	  size_t maxSupp = 0;
	  iterator newEnd = begin;
	  for (iterator it = begin; it != end; ++it) {
		const size_t supp = Ops::getSizeOfSupport(*it, varCount);
		if (maxSupp > supp)
		  continue;
		if (maxSupp < supp) {
		  maxSupp = supp;
		  newEnd = begin;
		}
		Ops::swap(*newEnd, *it, varCount);
		++newEnd;
	  }
	  return newEnd;
	}

	static const char* staticGetName() {
	  return "maxsupp";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class GenMinSupport : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceGenSplit(state.getIdeal().getMinSupportGen(), newState);	
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  const size_t varCount = divCounts.size();
	  size_t minSupp = varCount;
	  iterator newEnd = begin;
	  for (iterator it = begin; it != end; ++it) {
		const size_t supp = Ops::getSizeOfSupport(*it, varCount);
		if (minSupp < supp)
		  continue;
		if (minSupp > supp) {
		  minSupp = supp;
		  newEnd = begin;
		}
		Ops::swap(*newEnd, *it, varCount);
		++newEnd;
	  }
	  return newEnd;
	}

	static const char* staticGetName() {
	  return "minsupp";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class GenAny  : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  state.inPlaceGenSplit(0, newState);	
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  return ++begin;
	}

	static const char* staticGetName() {
	  return "any";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class GenRandom  : public GenStrategy {
  public:
	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  size_t pivotIndex = rand() % state.getIdeal().getGeneratorCount();
	  state.inPlaceGenSplit(pivotIndex, newState);	
	}

	virtual iterator filter(iterator begin, iterator end,
							const vector<size_t>& divCounts) {
	  const size_t varCount = divCounts.size();
	  const size_t genCount = end - begin;
	  const size_t choice = rand() % genCount;
	  Ops::swap(*begin, *(begin + choice), varCount);
	  return ++begin;
	}

	static const char* staticGetName() {
	  return "random";
	}

	virtual void getName(ostream& out) const {
	  out << staticGetName();
	}
  };

  class HybridPivotStrategy : public PivotStrategy {
  public:
	HybridPivotStrategy(auto_ptr<PivotStrategy> stdStrat,
						auto_ptr<PivotStrategy> genStrat):
	  _stdStrat(stdStrat), _genStrat(genStrat) {}

	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  size_t varToGenRatio =
		state.getNonEliminatedVarCount() /
		state.getIdeal().getGeneratorCount();
	  if (varToGenRatio < 10)
		_stdStrat->doPivot(state, newState, divCounts);
	  else
		_genStrat->doPivot(state, newState, divCounts);
	}

	virtual void getName(ostream& out) const {
	  out << "hybrid (";
	  _stdStrat->getName(out);
	  out << ", ";
	  _genStrat->getName(out);
	  out << ')';
	}

	virtual void computationCompleted(const PivotEulerAlg& alg) {
	  _stdStrat->computationCompleted(alg);
	  _genStrat->computationCompleted(alg);
	}

  private:
	auto_ptr<PivotStrategy> _stdStrat;
	auto_ptr<PivotStrategy> _genStrat;  
  };

  class DebugStrategy : public PivotStrategy {
  public:
	DebugStrategy(auto_ptr<PivotStrategy> strat, FILE* out):
	  _strat(strat), _out(out) {}

	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  const char* str1 = "\n\n\n"
		"********************************(debug output)********************************\n"
		"********** Processing this simplified state that is not a base case **********\n";
	  fputs(str1, _out);
	  state.print(_out);

	  _strat->doPivot(state, newState, divCounts);

	  const char* str2 =
		"<<<<<<<<<<<<<<<<<<<<<<<<<<<< Substate 1 of 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	  fputs(str2, _out);
	  state.print(_out);

	  const char* str3 =
		"<<<<<<<<<<<<<<<<<<<<<<<<<<<< Substate 2 of 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	  fputs(str3, _out);
	  newState.print(_out);
	}

	virtual void getName(ostream& out) const {
	  _strat->getName(out);
	}

	virtual void computationCompleted(const PivotEulerAlg& alg) {
	  _strat->computationCompleted(alg);
	  fputs("Debug: Euler characteristic computation completed.\n", _out);
	  gmp_fprintf(_out, "Debug: Computed Euler characteristics was %Zd.\n",
				  alg.getComputedEulerCharacteristic().get_mpz_t());
	}

  private:
	auto_ptr<PivotStrategy> _strat;
	FILE* _out;
  };

  class StatisticsStrategy : public PivotStrategy {
  public:
	StatisticsStrategy(auto_ptr<PivotStrategy> strat, FILE* out):
	  _strat(strat), _out(out), _statesSplit(0) {}

	virtual void doPivot(EulerState& state,
						 EulerState& newState,
						 const vector<size_t>& divCounts) {
	  ++_statesSplit;
	  _strat->doPivot(state, newState, divCounts);
	}

	virtual void getName(ostream& out) const {
	  _strat->getName(out);
	}

	virtual void computationCompleted(const PivotEulerAlg& alg) {
	  _strat->computationCompleted(alg);
	  fputs("******** Statistics for Euler characteristic computation *****\n", _out);
	  fprintf(_out, "* Using unique div simplify: %s\n",
			  alg.getUseUniqueDivSimplify() ? "yes" : "no");
	  fprintf(_out, "* Using many div simplify: %s\n",
			  alg.getUseManyDivSimplify() ? "yes" : "no");
	  fprintf(_out, "* Using implied div simplify: %s\n",
			  alg.getUseAllPairsSimplify() ? "yes" : "no");
	  ostringstream strategyName;
	  getName(strategyName);
	  fprintf(_out, "* Pivot strategy: %s\n", strategyName.str().c_str());

	  // The computation is a binary tree and we only see the internal
	  // nodes that each generate two more nodes. The number of leaves
	  // in a binary tree is one plus the number of internal nodes.
	  unsigned long totalStates = 2 * _statesSplit + 1;
	  fprintf(_out, "* States processed: %lu\n", (unsigned long)totalStates);
	  fputs("********\n", _out);
	}

  private:
	auto_ptr<PivotStrategy> _strat;
	FILE* _out;
	unsigned long _statesSplit;
  };

  typedef NameFactory<StdStrategy> StdFactory;
  StdFactory getStdStratFactory() {
    StdFactory factory("standard pivot strategy");
    nameFactoryRegister<StdRandom>(factory);
    nameFactoryRegister<StdAny>(factory);
    nameFactoryRegister<StdPopVar>(factory);
    nameFactoryRegister<StdPopGcd>(factory);
    nameFactoryRegister<StdRareVar>(factory);
    return factory;
  }

  typedef NameFactory<GenStrategy> GenFactory;
  GenFactory getGenStratFactory() {
    GenFactory factory("generator pivot strategy");
    nameFactoryRegister<GenRareVar>(factory);
    nameFactoryRegister<GenRarestVars>(factory);
    nameFactoryRegister<GenPopVar>(factory);
    nameFactoryRegister<GenMaxSupport>(factory);
    nameFactoryRegister<GenMinSupport>(factory);
    nameFactoryRegister<GenAny>(factory);
    nameFactoryRegister<GenRandom>(factory);
    return factory;
  }
}

auto_ptr<PivotStrategy> newStdPivotStrategy(const string& name) {
  if (name.compare(0, 6, "widen_") != 0) {
	auto_ptr<StdStrategy> strat = getStdStratFactory().createNoThrow(name);
	return auto_ptr<PivotStrategy>(strat.release());
  }

  auto_ptr<StdStrategy> subStrat =
	getStdStratFactory().createNoThrow(name.substr(6, name.size() - 6));
  if (subStrat.get() == 0)
	return auto_ptr<PivotStrategy>();
  return auto_ptr<PivotStrategy>(new StdWiden(subStrat));
}

auto_ptr<PivotStrategy> newGenPivotStrategy(const string& name) {
  GenFactory factory = getGenStratFactory();
  if (name.find('_') == string::npos) {
	auto_ptr<GenStrategy> strat = factory.createNoThrow(name);
	return auto_ptr<PivotStrategy>(strat.release());
  }

  auto_ptr<GenComposite> composite(new GenComposite());

  size_t pos = 0;
  string part;
  do {
	size_t nextPos = name.find('_', pos);
	if (nextPos == string::npos) {
	  part = name.substr(pos, string::npos);
	  pos = string::npos;
	} else {
	  part = name.substr(pos, nextPos - pos);
	  pos = nextPos + 1;
	}

	auto_ptr<GenStrategy> strat = factory.createNoThrow(part);
	if (strat.get() == 0)
	  return auto_ptr<PivotStrategy>();
	composite->addStrategy(strat);
  } while (pos != string::npos);
  return auto_ptr<PivotStrategy>(composite.release());
}

auto_ptr<PivotStrategy> newHybridPivotStrategy
(auto_ptr<PivotStrategy> stdStrat, auto_ptr<PivotStrategy> genStrat) {
  PivotStrategy* strat = new HybridPivotStrategy(stdStrat, genStrat);
  return auto_ptr<PivotStrategy>(strat);
}

auto_ptr<PivotStrategy> newDebugPivotStrategy(auto_ptr<PivotStrategy> strat,
											  FILE* out) {
  return auto_ptr<PivotStrategy>(new DebugStrategy(strat, out));
}

auto_ptr<PivotStrategy> newStatisticsPivotStrategy
(auto_ptr<PivotStrategy> strat, FILE* out) {
  return auto_ptr<PivotStrategy>(new StatisticsStrategy(strat, out));
}

auto_ptr<PivotStrategy> newDefaultPivotStrategy() {
  return newStdPivotStrategy("pivot");
}
