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
#include "SliceFacade.h"

#include "BigTermConsumer.h"
#include "CoefBigTermConsumer.h"
#include "TermTranslator.h"
#include "BigIdeal.h"
#include "Ideal.h"
#include "Term.h"
#include "MsmStrategy.h"
#include "TranslatingTermConsumer.h"
#include "DebugStrategy.h"
#include "SliceAlgorithm.h"
#include "DecomRecorder.h"
#include "TermGrader.h"
#include "FrobeniusStrategy.h"

SliceFacade::SliceFacade(const BigIdeal& ideal,
						 BigTermConsumer* consumer,
						 bool printActions):
  Facade(printActions),
  _printDebug(false), 
  _printStatistics(false),
  _useIndependence(false),
  _isMinimallyGenerated(false),
  _termConsumer(consumer),
  _coefTermConsumer(0),
  _generatedTermConsumer(0),
  _translator(0),
  _ideal(0) {
  ASSERT(consumer != 0);

  initialize(ideal);
}

SliceFacade::SliceFacade(const BigIdeal& ideal,
						 CoefBigTermConsumer* consumer,
						 bool printActions):
  Facade(printActions),
  _printDebug(false), 
  _printStatistics(false),
  _useIndependence(false),
  _isMinimallyGenerated(false),
  _termConsumer(0),
  _coefTermConsumer(consumer),
  _generatedTermConsumer(0),
  _translator(0),
  _ideal(0) {
  ASSERT(consumer != 0);

  initialize(ideal);
}

SliceFacade::~SliceFacade() {
  delete _generatedTermConsumer;
  delete _translator;
  delete _ideal;
}

void SliceFacade::setPrintDebug(bool printDebug) {
  _printDebug = printDebug;
}

void SliceFacade::setPrintStatistics(bool printStatistics) {
  _printStatistics = printStatistics;
}

void SliceFacade::setUseIndependence(bool useIndependence) {
  _useIndependence = useIndependence;
}

void SliceFacade::setIsMinimallyGenerated(bool isMinimallyGenerated) {
  _isMinimallyGenerated = isMinimallyGenerated;
}

bool SliceFacade::setSplitStrategy(const char* strategyName,
								   bool allowLabelSplits) {
  _strategy = strategyName;
  return true; // TODO: perform proper check.
}

void SliceFacade::computeMultigradedHilbertSeries() {
  minimize();

  beginAction("Computing multigraded Hilbert-Poincare series.");
  // TODO
  endAction();
}

void SliceFacade::computeUnivariateHilbertSeries() {
  minimize();

  beginAction("Computing univariate Hilbert-Poincare series.");
  fputs("Not implemented yet.", stderr);
  exit(1);
  // TODO
  endAction();
}

void SliceFacade::computeIrreducibleDecomposition() {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);

  if (_ideal->containsIdentity()) {
	getTermConsumer()->consume(Term(_ideal->getVarCount()));
	return;
  }

  if (_ideal->isZeroIdeal())
	return;

  beginAction("Preparing to compute irreducible decomposition.");

  _translator->addArtinianPowers(*_ideal); // TODO: rename to addFakePurePowers

  endAction();

  computeMaximalStaircaseMonomials();
}

void SliceFacade::computeMaximalStaircaseMonomials() {
  ASSERT(_ideal != 0);

  minimize();

  beginAction("Computing maximal staircase monomials.");

  SliceStrategy* strategy =
	MsmStrategy::newDecomStrategy(_strategy, getTermConsumer());
  runSliceAlgorithmAndDeleteStrategy(strategy);

  endAction();
}

void SliceFacade::computeMaximalStandardMonomials() {
  ASSERT(_translator != 0);

  beginAction("Preparing to compute maximal standard monomials.");

  _translator->decrement();

  endAction();

  computeMaximalStaircaseMonomials();
}

void SliceFacade::computeAlexanderDual(const vector<mpz_class>& point) {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);
  ASSERT(point.size() == _ideal->getVarCount());

  minimize();

  beginAction("Ensuring specified point is divisible by lcm.");

  vector<mpz_class> lcm;
  getLcmOfIdeal(lcm);

  for (size_t var = 0; var < lcm.size(); ++var) {
	if (lcm[var] > point[var]) {
	  endAction();
	  fputs("ERROR: The specified point to dualize on"
			"is not divisible by the\n"
			"least common multiple of the minimal"
			"generators of the ideal.\n", stderr);
	  exit(1);
	}
  }

  endAction();

  beginAction("Preparing to compute Alexander dual.");

  _translator->dualize(point);

  endAction();

  computeIrreducibleDecomposition();
}

void SliceFacade::computeAlexanderDual() {
  ASSERT(_ideal != 0);

  minimize();

  beginAction("Computing lcm for Alexander dual.");

  vector<mpz_class> lcm;
  getLcmOfIdeal(lcm);

  endAction();

  computeAlexanderDual(lcm);
}

void SliceFacade::computeAssociatedPrimes() {
  ASSERT(_ideal != 0);

  size_t varCount = _ideal->getVarCount();

  Ideal decom(varCount);
  {
	_generatedTermConsumer = new DecomRecorder(&decom);
	computeIrreducibleDecomposition();
	delete _generatedTermConsumer;
	_generatedTermConsumer = 0;
  }

  beginAction("Computing associated primes from irreducible decomposition.");

  Ideal radical(varCount);
  Term tmp(varCount);

  Ideal::const_iterator stop = decom.end();
  for (Ideal::const_iterator it = decom.begin(); it != stop; ++it) {
    for (size_t var = 0; var < varCount; ++var) {
      if (_translator->getExponent(var, (*it)[var]) == 0)
		tmp[var] = 0;
      else
		tmp[var] = 1;
    }
    radical.insert(tmp);
  }
  decom.clear();

  radical.removeDuplicates();

  {
	// Construct translator for zero and one.

	BigIdeal zeroOneIdeal(_translator->getNames());
	zeroOneIdeal.newLastTerm(); // Add term with all exponents zero.
	zeroOneIdeal.newLastTerm(); // Add term with all exponents one.
	for (size_t var = 0; var < varCount; ++var)
	  zeroOneIdeal.getLastTermExponentRef(var) = 1;

	Ideal dummy;
	TermTranslator zeroOneTranslator(zeroOneIdeal, dummy, false);

	Term tmp(varCount);
	Ideal::const_iterator stop = radical.end();
	for (Ideal::const_iterator it = radical.begin(); it != stop; ++it) {
	  tmp = *it;
	  _termConsumer->consume(tmp, &zeroOneTranslator);
	}
  }

  endAction();
}

void SliceFacade::solveStandardProgram(const vector<mpz_class>& grading,
									   bool useBound) {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);
  ASSERT(grading.size() == _ideal->getVarCount());

  minimize();

  beginAction("Solving maximal standard monomial optimization program.");

  _translator->decrement();
  TermGrader grader(grading, _translator);
  MsmStrategy* strategy = FrobeniusStrategy::newFrobeniusStrategy
	(_strategy, getTermConsumer(), grader, useBound);
  runSliceAlgorithmAndDeleteStrategy(strategy);

  endAction();
}

void SliceFacade::initialize(const BigIdeal& ideal) {
  // Only call once.
  ASSERT(_ideal == 0);
  ASSERT(_translator == 0);

  beginAction("Translating ideal to internal data structure.");

  _ideal = new Ideal(ideal.getVarCount());
  _translator = new TermTranslator(ideal, *_ideal, false);

  endAction();
}

void SliceFacade::minimize() {
  ASSERT(_ideal != 0);

  if (_isMinimallyGenerated)
	return;

  beginAction("Minimizing ideal.");

  _ideal->minimize();
  _isMinimallyGenerated = true;

  endAction();
}

void SliceFacade::getLcmOfIdeal(vector<mpz_class>& bigLcm) {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);

  Term lcm(_ideal->getVarCount());
  _ideal->getLcm(lcm);

  bigLcm.clear();
  for (size_t var = 0; var < _ideal->getVarCount(); ++var)
	bigLcm.push_back(_translator->getExponent(var, lcm));
}

void SliceFacade::runSliceAlgorithmAndDeleteStrategy(SliceStrategy* strategy) {
  strategy->setUseIndependence(_useIndependence);

  if (_printDebug)
	strategy = new DebugStrategy(strategy, stderr);

  // TODO: implement collection and printing of statistics.

  ::runSliceAlgorithm(*_ideal, strategy);
  delete strategy;
}

TermConsumer* SliceFacade::getTermConsumer() {
  ASSERT(_termConsumer != 0);

  if (_generatedTermConsumer == 0)
	_generatedTermConsumer =
	  new TranslatingTermConsumer(_termConsumer, _translator);

  return _generatedTermConsumer;
}
