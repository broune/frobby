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
#include "HilbertStrategy.h"

#include "Term.h"
#include "HilbertSlice.h"
#include "Ideal.h"
#include "CoefTermConsumer.h"
#include "Projection.h"
#include "IndependenceSplitter.h"

#include "SliceEvent.h"

HilbertStrategy::HilbertStrategy(bool useIndependence):
  _useIndependence(useIndependence) {
}

// TODO: move elsewhere
class HilbertIndependenceConsumer : public CoefTermConsumer,
									public SliceEvent {
public:
  HilbertIndependenceConsumer(HilbertStrategy* strategy):
	_rightConsumer(this),
	_strategy(strategy) {
	ASSERT(strategy != 0);
	clear();
  }

  void reset(CoefTermConsumer* parent,
			 IndependenceSplitter& splitter,
			 size_t varCount) {
	ASSERT(parent != 0);

	_tmpTerm.reset(varCount);
	_parent = parent;

	splitter.getBigProjection(_leftProjection);
	splitter.getRestProjection(_rightProjection);

	_rightTerms.clearAndSetVarCount(_rightProjection.getRangeVarCount());
  }

  void clear() {
	_parent = 0;
	_rightTerms.clear();
	_rightCoefs.clear();
  }

  virtual void raiseEvent() {
	ASSERT(_parent != 0);
	clear();

	ASSERT(_strategy != 0);
	_strategy->freeConsumer(this);
  }

  CoefTermConsumer* getLeftConsumer() {
	ASSERT(_parent != 0);
	return this;
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(_parent != 0);
	consumeLeft(coef, term);
  }

  CoefTermConsumer* getRightConsumer() {
	ASSERT(_parent != 0);
	return &_rightConsumer;
  }

  const Projection& getLeftProjection() const {
	ASSERT(_parent != 0);
	return _leftProjection;
  }

  const Projection& getRightProjection() const {
	ASSERT(_parent != 0);
	return _rightProjection;
  }

private:
  class RightConsumer : public CoefTermConsumer {
  public:
	RightConsumer(HilbertIndependenceConsumer* parent):
	  _parent(parent) {
	}

	virtual void consume(const mpz_class& coef, const Term& term) {
	  _parent->consumeRight(coef, term);
	}

  private:
	HilbertIndependenceConsumer* _parent;
  };

  virtual void consumeLeft(const mpz_class& leftCoef, const Term& leftTerm) {
	ASSERT(_tmpTerm.getVarCount() ==
		   _leftProjection.getRangeVarCount() +
		   _rightProjection.getRangeVarCount());

	_leftProjection.inverseProject(_tmpTerm, leftTerm);

	size_t rightSize = _rightTerms.getGeneratorCount();
	ASSERT(rightSize == _rightCoefs.size());
	for (size_t right = 0; right < rightSize; ++right) {
	  _rightProjection.inverseProject(_tmpTerm,
									  *(_rightTerms.begin() + right));
	  // _rightTerms[right]);
	  _tmpCoef = leftCoef * _rightCoefs[right];

	  _parent->consume(_tmpCoef, _tmpTerm);
	}
  }

  virtual void consumeRight(const mpz_class& coef, const Term& term) {
	ASSERT(term.getVarCount() == _rightProjection.getRangeVarCount());
	ASSERT(_rightTerms.getVarCount() == term.getVarCount());

	_rightTerms.insert(term);
	_rightCoefs.push_back(coef);
  }

  Term _tmpTerm;
  mpz_class _tmpCoef;

  CoefTermConsumer* _parent;
  Projection _leftProjection;
  Projection _rightProjection;

  Ideal _rightTerms;
  vector<mpz_class> _rightCoefs;

  RightConsumer _rightConsumer;

  HilbertStrategy* _strategy;
};

HilbertStrategy::~HilbertStrategy() {
  while (!_sliceCache.empty()) {
	delete _sliceCache.back();
	_sliceCache.pop_back();
  }

  while (!_consumerCache.empty()) {
	delete _consumerCache.back();
	_consumerCache.pop_back();
  }
}

HilbertSlice* HilbertStrategy::setupInitialSlice(const Ideal& ideal,
												 CoefTermConsumer* consumer) {
  ASSERT(consumer != 0);
  _term.reset(ideal.getVarCount());

  size_t varCount = ideal.getVarCount();
  Ideal sliceIdeal(varCount);

  if (!ideal.contains(Term(varCount))) {
	consumer->consume(1, Term(varCount));

	if (ideal.getGeneratorCount() > 0) {
	  Term allOnes(varCount);
	  for (size_t var = 0; var < varCount; ++var)
		allOnes[var] = 1;

	  sliceIdeal = ideal;
	  sliceIdeal.product(allOnes);
	}
  }

  HilbertSlice* slice = new HilbertSlice(sliceIdeal, Ideal(varCount),
										 Term(varCount), consumer);
  slice->simplify();
  return slice;
}

pair<HilbertSlice*, HilbertSlice*>
HilbertStrategy::split(HilbertSlice* slice, SliceEvent*& event) {
  event = 0;
  pair<HilbertSlice*, HilbertSlice*> slicePair;
  if (_useIndependence && independenceSplit(slice, slicePair, event))
	return slicePair;

  Term _term(slice->getVarCount());
  getPivot(_term, *slice);

  ASSERT(_term.getVarCount() == slice->getVarCount());
  ASSERT(!_term.isIdentity()); 
  ASSERT(!slice->getIdeal().contains(_term));
  ASSERT(!slice->getSubtract().contains(_term));

  // Compute inner slice.
  HilbertSlice* inner = newSlice();
  *inner = *slice;
  inner->innerSlice(_term);
  inner->simplify();

  // Compute outer slice.
  slice->outerSlice(_term);
  slice->simplify();

  slicePair.first = inner;
  slicePair.second = slice;

  // Process smaller slices before larger ones to preserve memory.
  if (slicePair.first->getIdeal().getGeneratorCount() <
	  slicePair.second->getIdeal().getGeneratorCount())
	swap(slicePair.first, slicePair.second);
  return slicePair;
}

void HilbertStrategy::freeSlice(HilbertSlice* slice) {
  ASSERT(slice != 0);

  slice->clear(); // To preserve memory.
  _sliceCache.push_back(slice);
}

HilbertSlice* HilbertStrategy::newSlice() {
  if (_sliceCache.empty())
	return new HilbertSlice();

  HilbertSlice* slice = _sliceCache.back();
  _sliceCache.pop_back();
  return slice;
}

void HilbertStrategy::getPivot(Term& term, HilbertSlice& slice) {
  ASSERT(term.getVarCount() == slice.getVarCount());

  // Get best (most populated) variable to split on. Term serves
  // double-duty as a container of the support counts.
  slice.getIdeal().getSupportCounts(term);

  const Term& lcm = slice.getLcm();
  for (size_t var = 0; var < slice.getVarCount(); ++var)
	if (lcm[var] <= 1)
	  term[var] = 0;

  ASSERT(!term.isIdentity());
  size_t bestVar = term.getFirstMaxExponent();

  // Get median positive exponent of bestVar.
  slice.singleDegreeSortIdeal(bestVar);
  Ideal::const_iterator end = slice.getIdeal().end();
  Ideal::const_iterator begin = slice.getIdeal().begin();
  while ((*begin)[bestVar] == 0) {
	++begin;
	ASSERT(begin != end);
  }
  term.setToIdentity();
  term[bestVar] = (*(begin + (distance(begin, end) ) / 2))[bestVar];
  if (term[bestVar] == lcm[bestVar])
	term[bestVar] -= 1;

  ASSERT(!term.isIdentity());
  ASSERT(!slice.getIdeal().contains(term));
  ASSERT(!slice.getSubtract().contains(term));
  ASSERT(term.strictlyDivides(slice.getLcm()));
}

void HilbertStrategy::freeConsumer(HilbertIndependenceConsumer* consumer) {
  ASSERT(consumer != 0);
  ASSERT(std::find(_consumerCache.begin(),
				   _consumerCache.end(), consumer) ==
		 _consumerCache.end());

  _consumerCache.push_back(consumer);
}

bool HilbertStrategy::independenceSplit
(HilbertSlice* slice, pair<HilbertSlice*, HilbertSlice*>& slicePair,
 SliceEvent*& event) {
  ASSERT(slice != 0);

  static IndependenceSplitter splitter; // TODO: get rid of static
  if (!splitter.analyze(*slice))
	return false;

  HilbertIndependenceConsumer* consumer = newConsumer();
  consumer->reset(slice->getConsumer(), splitter, slice->getVarCount());
  event = consumer;

  HilbertSlice* leftSlice = newSlice();
  leftSlice->setToProjOf(*slice, consumer->getLeftProjection(),
						 consumer->getLeftConsumer());

  HilbertSlice* rightSlice = newSlice();
  rightSlice->setToProjOf(*slice, consumer->getRightProjection(),
						  consumer->getRightConsumer());

  slicePair.first = leftSlice;
  slicePair.second = rightSlice;

  freeSlice(slice);

  return true;
}

HilbertIndependenceConsumer* HilbertStrategy::newConsumer() {
  if (_consumerCache.empty())
	return new HilbertIndependenceConsumer(this);

  HilbertIndependenceConsumer* consumer = _consumerCache.back();
  _consumerCache.pop_back();
  return consumer;
}
