/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "BigattiBaseCase.h"

#include "BigattiState.h"
#include "TermTranslator.h"
#include <algorithm>

BigattiBaseCase::BigattiBaseCase(const TermTranslator& translator):
  _maxCount(translator.getVarCount()),
 _lcm(translator.getVarCount()),
 _outputMultivariate(translator.getVarCount()),
 _computeUnivariate(false),
 _translator(translator),
 _totalBaseCasesEver(0),
 _totalTermsOutputEver(0),
 _printDebug(false) {
}

bool BigattiBaseCase::genericBaseCase(const BigattiState& state) {
  if (baseCase(state))
    return true;

  if (!state.getIdeal().isWeaklyGeneric())
    return false;

  enumerateScarfComplex(state, false);

  ++_totalBaseCasesEver;
  return true;
}

bool BigattiBaseCase::baseCase(const BigattiState& state) {
  ASSERT(_maxCount.size() == state.getVarCount());

  if (simpleBaseCase(state))
    return true;

  if (state.getIdeal().getGeneratorCount() > state.getVarCount())
    return false;

  state.getIdeal().getLcm(_lcm);
  if (state.getIdeal().getGeneratorCount() > _lcm.getSizeOfSupport())
    return false;

  fill(_maxCount.begin(), _maxCount.end(), 0);
  Ideal::const_iterator end = state.getIdeal().end();
  Ideal::const_iterator it = state.getIdeal().begin();
  for (; it != end; ++it) {
    bool hasMax = false;
    for (size_t var = 0; var < state.getVarCount(); ++var) {
      ASSERT((*it)[var] <= _lcm[var]);
      if ((*it)[var] == _lcm[var] && _lcm[var] > 0) {
        hasMax = true;
        _maxCount[var] += 1;
        if (_maxCount[var] > 1)
          return false;
      }
    }
    if (!hasMax)
      return false;
  }

  enumerateScarfComplex(state, true);

  ++_totalBaseCasesEver;
  return true;
}

void BigattiBaseCase::output(bool plus, const Term& term) {
  if (_printDebug) {
    fputs("Debug: Outputting term ", stderr);
    fputc(plus ? '+' : '-', stderr);
    term.print(stderr);
    fputs(".\n", stderr);
  }

  ++_totalTermsOutputEver;
  if (_computeUnivariate) {
    if (term.getVarCount() == 0)
      _tmp = 0;
    else
      _tmp = _translator.getExponent(0, term);
    for (size_t var = 1; var < term.getVarCount(); ++var)
      _tmp += _translator.getExponent(var, term);
    _outputUnivariate.add(plus, _tmp);
  } else
    _outputMultivariate.add(plus, term);
}

void BigattiBaseCase::feedOutputTo
(CoefBigTermConsumer& consumer,
 bool inCanonicalOrder) {
  if (_computeUnivariate)
    _outputUnivariate.feedTo(consumer, inCanonicalOrder);
  else
    _outputMultivariate.feedTo(_translator, consumer, inCanonicalOrder);
}

void BigattiBaseCase::setPrintDebug(bool value) {
  _printDebug = value;
}

void BigattiBaseCase::setComputeUnivariate(bool value) {
  _computeUnivariate = value;
}

size_t BigattiBaseCase::getTotalBaseCasesEver() const {
  return _totalBaseCasesEver;
}

size_t BigattiBaseCase::getTotalTermsOutputEver() const {
  return _totalTermsOutputEver;
}

size_t BigattiBaseCase::getTotalTermsInOutput() const {
  if (_computeUnivariate)
    return _outputUnivariate.getTermCount();
  else
    return _outputMultivariate.getTermCount();
}

bool BigattiBaseCase::simpleBaseCase(const BigattiState& state) {
  const Ideal& ideal = state.getIdeal();
  size_t genCount = ideal.getGeneratorCount();
  const Term& multiply = state.getMultiply();

  if (genCount > 2)
    return false;

  output(true, multiply);
  if (genCount == 0)
    return true;

  _lcm.product(multiply, ideal[0]);
  output(false, _lcm);
  if (genCount == 1)
    return true;

  ASSERT(genCount == 2);
  _lcm.product(multiply, ideal[1]);
  output(false, _lcm);

  _lcm.lcm(ideal[0], ideal[1]);
  _lcm.product(_lcm, multiply);
  output(true, _lcm);

  ++_totalBaseCasesEver;
  return true;
}

bool BigattiBaseCase::univariateAllFaces(const BigattiState& state) {
  ASSERT(_computeUnivariate);
  const Ideal& ideal = state.getIdeal();
  const Term& multiply = state.getMultiply();

  if (!ideal.disjointSupport())
    return false;

  if (ideal.getGeneratorCount() > 30)
    return false; // Coefficients may not fit in 32 bits

  Term max(ideal.getVarCount());
  ideal.getLcm(max);
  max.product(max, multiply);

  _tmp = 0;
  for (size_t var = 0; var < max.getVarCount(); ++var)
    _tmp += _translator.getExponent(var, max);
  if (_tmp > 1024*1024)
    return false; // Too high memory requirement.
  ASSERT(_tmp.fits_uint_p());
  size_t maxDegree = _tmp.get_ui();

  size_t approxWorkForScarfComplex = (1 << ideal.getGeneratorCount());
  if (approxWorkForScarfComplex < maxDegree)
    return false; // Scarf complex is on par or faster.

  // At this point we have made sure that this instance makes sense to
  // solve using this method. We have also determined that we can do
  // everything in machine ints.

  vector<int> poly;
  poly.reserve(maxDegree);
  poly.push_back(1);

  // TODO: sort with smallest first to decrease work in early
  // iterations.
  for (size_t i = 0; i < ideal.getGeneratorCount(); ++i) {
    ASSERT(poly.back() != 0);
    const Exponent* gen = ideal[i];

    // calculate degree
    int degree = 0;
    for (size_t var = 0; var < max.getVarCount(); ++var)
      degree += _translator.getExponent(var, multiply[var] + gen[var]).get_ui()
        - _translator.getExponent(var, multiply[var]).get_ui();

    // replace poly P by (1-t^degree)*P = P-P*t^degree.
    size_t oldSize = poly.size();
    poly.resize(oldSize + degree);
    for (size_t e = oldSize; e > 0;) {
      --e;
      poly[e+degree] -= poly[e];
    }
  }

  int degree = 0;
  for (size_t var = 0; var < max.getVarCount(); ++var)
    degree += _translator.getExponent(var, multiply).get_ui();

  for (size_t e = 0; e < poly.size(); ++e) {
    if (_printDebug) {
      fprintf(stderr, "Debug: Outputting term %i*t^%u.\n",
              poly[e], (unsigned int)(e + degree));
    }

    ++_totalTermsOutputEver;
    _outputUnivariate.add(poly[e], e+degree);
  }

  return true;
}

void BigattiBaseCase::enumerateScarfComplex(const BigattiState& state,
                                            bool allFaces) {
  if (allFaces &&
      _computeUnivariate &&
      univariateAllFaces(state)) {
    return;
  }

  const Ideal& ideal = state.getIdeal();

  // Set up _states with enough entries of the right size.
  size_t needed = ideal.getGeneratorCount() + 1;
  if (_states.size() < needed)
    _states.resize(needed);
  for (size_t i = 0; i < _states.size(); ++i)
    _states[i].term.reset(state.getVarCount());

  // Set up the initial state
  ASSERT(!ideal.isZeroIdeal());
  _states[0].plus = true;
  _states[0].pos = ideal.begin();
  ASSERT(_states[0].term.isIdentity());

  // Cache this to avoid repeated calls to end().
  Ideal::const_iterator stop = ideal.end();

  // Iterate until all states are done. The active entries of _states
  // are those from index 0 up to and including _current.
  size_t current = 0;
  while (true) {
    ASSERT(current < _states.size());
    State& currentState = _states[current];

    if (currentState.pos == stop) {
      // This is a base case since we have considered all minimal
      // generators.
      _lcm.product(currentState.term, state.getMultiply());
      output(currentState.plus, _lcm);

      // We are done with this entry, so go back to the previous
      // active entry.
      if (current == 0)
        break; // Nothing remains to be done.
      --current;
    } else {
      // Split into two cases according to whether we put the minimal
      // generator at pos into the face or not.
      ASSERT(current + 1 < _states.size());
      State& next = _states[current + 1];

      next.term.lcm(currentState.term, *currentState.pos);
      ++currentState.pos;

      if (allFaces || !ideal.strictlyContains(next.term)) {
        // If allFaces is true we do not need to check the condition
        // since we know it should always hold.
        ASSERT(!ideal.strictlyContains(next.term));

        next.plus = !currentState.plus;
        next.pos = currentState.pos;
        ++current;
      }
    }
  }
}
