/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
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
#include "ScarfHilbertAlgorithm.h"

#include "Ideal.h"
#include "Term.h"
#include "CoefTermConsumer.h"
#include "TermTranslator.h"
#include "Deformer.h"
#include "CoefBigTermConsumer.h"
#include "HashPolynomial.h"
#include "UniHashPolynomial.h"
#include "CommonParams.h"

class UndeformConsumer : public CoefTermConsumer {
public:
  UndeformConsumer(Ideal& toDeform,
				   const TermTranslator& translator,
				   CoefBigTermConsumer& consumer,
				   bool univar,
				   bool canonical):
	_univar(univar),
	_tmp(toDeform.getVarCount()),
	_deformer(toDeform),
	_translator(translator),
	_canonical(canonical),
	_consumer(consumer),
	_poly(toDeform.getVarCount()) {
  }

  virtual void consumeRing(const VarNames& names) {
	ASSERT(names == _translator.getNames());
  }

  virtual void beginConsuming() {
  }

  virtual void consume(const mpz_class& coef, const Term& term) {
	ASSERT(term.getVarCount() == _tmp.getVarCount());
	_tmp = term;
	_deformer.undeform(_tmp);

	if (_univar) {
	  if (_tmp.getVarCount() == 0)
		_tdeg = 0;
	  else
		_tdeg = _translator.getExponent(0, _tmp);
	  for (size_t var = 1; var < _tmp.getVarCount(); ++var)
		_tdeg += _translator.getExponent(var, _tmp);
	  _uniPoly.add(coef, _tdeg);
	} else 
	  _poly.add(coef, _tmp);
  }

  virtual void doneConsuming() {
	if (_univar)
	  _uniPoly.feedTo(_consumer, _canonical);
	else
	  _poly.feedTo(_translator, _consumer, _canonical);
  }

private:
  bool _univar;
  Term _tmp;
  Deformer _deformer;
  const TermTranslator& _translator;
  bool _canonical;
  CoefBigTermConsumer& _consumer;
  HashPolynomial _poly;
  UniHashPolynomial _uniPoly;
  mpz_class _tdeg;
};

ScarfHilbertAlgorithm::ScarfHilbertAlgorithm
(const TermTranslator& translator,
 const CommonParams& params):
  _translator(translator),
  _params(params) {
}

void ScarfHilbertAlgorithm::runGeneric(const Ideal& ideal,
									   CoefBigTermConsumer& consumer,
									   bool univariate,
									   bool canonical) {
  Ideal deformed(ideal);
  UndeformConsumer undeformer
	(deformed, _translator, consumer, univariate, canonical);

  undeformer.consumeRing(_translator.getNames());
  undeformer.beginConsuming();
  enumerateScarfComplex(deformed, undeformer, false);
  undeformer.doneConsuming();
}

struct State {
  Term term;
  Ideal::const_iterator pos;
  bool plus;
};

// TODO: _state should be member.
void ScarfHilbertAlgorithm::enumerateScarfComplex(const Ideal& ideal,
												  CoefTermConsumer& consumer,
												  bool everythingIsAFace) {
  ASSERT(Ideal(ideal).isStronglyGeneric());
  if (_params.getPrintDebug()) {
	fputs("Enumerating faces of Scarf complex of:\n", stderr);
	ideal.print(stderr);
  }

  vector<State> _states; // make member

  // Set up _states with enough entries of the right size.
  size_t needed = ideal.getGeneratorCount() + 1; 
  if (_states.size() < needed)
	_states.resize(needed);
  for (size_t i = 0; i < _states.size(); ++i)
	_states[i].term.reset(ideal.getVarCount());

  // Set up the initial state
  _states[0].plus = true;
  _states[0].pos = ideal.begin();
  ASSERT(_states[0].term.isIdentity());

  // Cache this to avoid repeated calls to end().
  Ideal::const_iterator idealEnd = ideal.end();

  // Iterate until all states are done. The active entries of _states
  // are those from index 0 up to and including index current.
  size_t current = 0;
  while (true) {
	ASSERT(current < _states.size());
	State& currentState = _states[current];

	if (_params.getPrintDebug()) {
	  fprintf(stderr,
			  "DEBUG: Looking at ideal index %u/%u, "
			  "%u jobs to go, and lcm(face)=",
			  static_cast<unsigned int>(currentState.pos - ideal.begin()),
			  static_cast<unsigned int>(ideal.getGeneratorCount()),
			  static_cast<unsigned int>(current));
	  currentState.term.print(stderr);
	  fputc('\n', stderr);
	  fflush(stderr);
	}

	if (currentState.pos == idealEnd) {
	  // We have considered all minimal generators so there are no
	  // more possibilities for extending this face to consider. It
	  // only remains to output the lcm of the face.
	  consumer.consume(currentState.plus ? 1 : -1, currentState.term);

	  // Go to the next entry at index current - 1.
	  if (current == 0)
		break; // Nothing remains to be done.
	  --current;
	} else {
	  ASSERT(current + 1 < _states.size());
	  State& next = _states[current + 1];

	  // We consider two cases:
	  // Case 1: Do not add the minimal generator at pos to the
	  // face. We do this in-place at currentPos.
	  //
	  // Case 2: Add the minimal generator at pos to the face. Record
	  // this possibility only if that is still a face.

	  next.term.lcm(currentState.term, *currentState.pos); // for Case 2
	  ++currentState.pos; // for Case 1, but used above so can't do it before
	  if (everythingIsAFace || !ideal.strictlyContains(next.term)) {
		ASSERT(!ideal.strictlyContains(next.term));
		// Case 2
		next.plus = !currentState.plus;
		next.pos = currentState.pos;
		++current;
	  }
	}
  }
}
