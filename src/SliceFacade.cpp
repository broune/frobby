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
#include "TranslatingCoefTermConsumer.h"
#include "DebugStrategy.h"
#include "SliceAlgorithm.h"
#include "DecomRecorder.h"
#include "TermGrader.h"
#include "FrobeniusStrategy.h"
#include "CanonicalCoefTermConsumer.h"
#include "HilbertStrategy.h"
#include "IOHandler.h"
#include "BigPolynomial.h"
#include "TotalDegreeCoefTermConsumer.h"
#include "CoefBigTermRecorder.h"
#include "CanonicalTermConsumer.h"
#include "VarSorter.h"
#include "StatisticsStrategy.h"
#include "error.h"

SliceFacade::SliceFacade(const BigIdeal& ideal,
						 BigTermConsumer* consumer,
						 bool printActions):
  Facade(printActions),
  _printDebug(false), 
  _printStatistics(false),
  _useIndependence(false),
  _isMinimallyGenerated(false),
  _canonicalOutput(false),
  _out(0),
  _ioHandler(0),
  _termConsumer(consumer),
  _coefTermConsumer(0),
  _generatedTermConsumer(0),
  _generatedCoefTermConsumer(0),
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
  _canonicalOutput(false),
  _out(0),
  _ioHandler(0),
  _termConsumer(0),
  _coefTermConsumer(consumer),
  _generatedTermConsumer(0),
  _generatedCoefTermConsumer(0),
  _translator(0),
  _ideal(0) {
  ASSERT(consumer != 0);

  initialize(ideal);
}

SliceFacade::SliceFacade(const BigIdeal& ideal,
						 IOHandler* handler,
						 FILE* out,
						 bool printActions):
  Facade(printActions),
  _printDebug(false), 
  _printStatistics(false),
  _useIndependence(false),
  _isMinimallyGenerated(false),
  _canonicalOutput(false),
  _out(out),
  _ioHandler(handler),
  _termConsumer(0),
  _coefTermConsumer(0),
  _generatedTermConsumer(0),
  _generatedCoefTermConsumer(0),
  _translator(0),
  _ideal(0) {
  ASSERT(handler != 0);
  ASSERT(out != 0);

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

void SliceFacade::setSplitStrategy(auto_ptr<SplitStrategy> split) {
  _split = split;
}

void SliceFacade::setCanonicalOutput() {
  ASSERT(_translator != 0);

  _canonicalOutput = true;

  VarSorter sorter(_translator->getNames());
  sorter.permute(_translator);

  Ideal::iterator stop = _ideal->end();
  for (Ideal::iterator it = _ideal->begin(); it != stop; ++it)
    sorter.permute(*it);
}

void SliceFacade::computeMultigradedHilbertSeries() {
  ASSERT(_ideal != 0);

  minimize();

  beginAction("Computing multigraded Hilbert-Poincare series.");

  CoefTermConsumer* consumer = getCoefTermConsumer();

  consumer->beginConsuming();
  SliceStrategy* strategy = new HilbertStrategy(consumer, _split.get());
  runSliceAlgorithmAndDeleteStrategy(strategy);
  consumer->doneConsuming();

  endAction();
}

void SliceFacade::computeUnivariateHilbertSeries() {
  minimize();

  ASSERT(_translator != 0);

  beginAction("Preparing to compute univariate Hilbert-Poincare series.");

  VarNames names;
  names.addVar("t");

  BigPolynomial polynomial(names);
  _generatedCoefTermConsumer.reset
	(new TotalDegreeCoefTermConsumer
	 (auto_ptr<CoefBigTermConsumer>(new CoefBigTermRecorder(&polynomial)),
	  _translator));

  endAction();

  computeMultigradedHilbertSeries();
  _generatedCoefTermConsumer.reset();

  beginAction("Output of computed univariate Hilbert-Poincare series.");

  if (_out != 0) {
	ASSERT(_ioHandler != 0);
	_ioHandler->writePolynomial(polynomial, _out);
  } else {
	ASSERT(_coefTermConsumer != 0);

	_coefTermConsumer->beginConsuming();
	size_t termCount = polynomial.getTermCount();
	for (size_t term = 0; term < termCount; ++term)
	  _coefTermConsumer->consume(polynomial.getCoef(term),
								 polynomial.getTerm(term));
	_coefTermConsumer->doneConsuming();
  }

  endAction();
}

void SliceFacade::computeIrreducibleDecomposition(bool encode) {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);

  minimize();

  if (!encode)
	doIrreducibleIdealOutput();

  beginAction("Preparing to compute irreducible decomposition.");

  _translator->addPurePowersAtInfinity(*_ideal);

  endAction();

  computeMaximalStaircaseMonomials();
}

void SliceFacade::computeMaximalStaircaseMonomials() {
  ASSERT(_ideal != 0);
  ASSERT(_split.get() != 0);

  minimize();

  beginAction("Computing maximal staircase monomials.");

  SliceStrategy* strategy = new MsmStrategy(getTermConsumer(), _split.get());
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
	  reportError
		("The specified point to dualize on is not divisible by the "
		 "least common multiple of the minimal generators of the ideal.");
	}
  }

  endAction();

  beginAction("Preparing to compute Alexander dual.");

  _translator->dualize(point);

  endAction();

  getTermConsumer(); // To ensure some output is generated.

  computeIrreducibleDecomposition(true);
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

  // Obtain generators of radical from irreducible decomposition.
  Ideal radical(varCount);
  {
	Ideal decom(varCount);
	_generatedTermConsumer = new DecomRecorder(&decom);
	computeIrreducibleDecomposition(true);
	delete _generatedTermConsumer;
	_generatedTermConsumer = 0;

	beginAction("Computing associated primes from irreducible decomposition.");

	Term tmp(varCount);
	Ideal::const_iterator stop = decom.end();
	for (Ideal::const_iterator it = decom.begin(); it != stop; ++it) {
	  for (size_t var = 0; var < varCount; ++var) {
		// We cannot just check whether (*it)[var] == 0, since the
		// added fake pure powers map to zero but are not themselves
		// zero.
		if (_translator->getExponent(var, (*it)[var]) == 0)
		  tmp[var] = 0;
		else
		  tmp[var] = 1;
	  }
	  radical.insert(tmp);
	}
  }

  radical.removeDuplicates();

  // Construct translator for zero and one.
  {
	BigIdeal zeroOneIdeal(_translator->getNames());
	zeroOneIdeal.newLastTerm(); // Add term with all exponents zero.
	zeroOneIdeal.newLastTerm(); // Add term with all exponents one.
	for (size_t var = 0; var < varCount; ++var)
	  zeroOneIdeal.getLastTermExponentRef(var) = 1;

	delete _translator;
	_translator = new TermTranslator(zeroOneIdeal, *_ideal, false);
  }

  // Output associated primes.
  TermConsumer* consumer = getTermConsumer();
  Term tmp(varCount);
  Ideal::const_iterator stop = radical.end();
  for (Ideal::const_iterator it = radical.begin(); it != stop; ++it) {
	tmp = *it;
	consumer->consume(tmp);
  }

  endAction();
}

bool SliceFacade::solveStandardProgram(const vector<mpz_class>& grading,
									   bool useBound) {
  ASSERT(_split.get() != 0);
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);
  ASSERT(grading.size() == _ideal->getVarCount());

  minimize();

  size_t varCount = _ideal->getVarCount();

  beginAction("Solving maximal standard monomial optimization program.");

  _translator->decrement();
  TermGrader grader(grading, _translator);

  Ideal solution(varCount);
  _generatedTermConsumer = new DecomRecorder(&solution);

  SliceStrategy* strategy =
	new FrobeniusStrategy(getTermConsumer(), grader, _split.get(), useBound);

  runSliceAlgorithmAndDeleteStrategy(strategy);

  delete _generatedTermConsumer;
  _generatedTermConsumer = 0;

  endAction();

  if (solution.isZeroIdeal())
	return false;

  getTermConsumer()->consume(Term(*solution.begin(), varCount));

  return true;
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

  if (_printStatistics)
	strategy = new StatisticsStrategy(strategy, stderr);

  ::runSliceAlgorithm(*_ideal, strategy);
  delete strategy;
}

void SliceFacade::doIrreducibleIdealOutput() {
  ASSERT(_translator != 0);
  ASSERT(_out != 0);
  ASSERT(_generatedTermConsumer == 0);
  ASSERT(_ioHandler != 0);
  ASSERT(_termConsumer == 0);

  _generatedTermConsumer =
	_ioHandler->createIrreducibleIdealWriter(_translator, _out);

  if (_canonicalOutput)
	_generatedTermConsumer = new CanonicalTermConsumer
	  (_generatedTermConsumer, _ideal->getVarCount(), _translator);

  ASSERT(_generatedTermConsumer != 0);
}

TermConsumer* SliceFacade::getTermConsumer() {
  ASSERT(_translator != 0);

  if (_generatedTermConsumer == 0) {
	if (_termConsumer != 0) {
	  _generatedTermConsumer =
		new TranslatingTermConsumer(_termConsumer, _translator);
	} else {
	  ASSERT(_ioHandler != 0);
	  ASSERT(_out != 0);
	  _generatedTermConsumer =
		_ioHandler->createIdealWriter(_translator, _out);
	}

	if (_canonicalOutput)
	  _generatedTermConsumer = new CanonicalTermConsumer
		(_generatedTermConsumer, _ideal->getVarCount(), _translator);
  }
  ASSERT(_generatedTermConsumer != 0);

  return _generatedTermConsumer;
}

CoefTermConsumer* SliceFacade::getCoefTermConsumer() {
  ASSERT(_ideal != 0);
  ASSERT(_translator != 0);

  if (_generatedCoefTermConsumer.get() == 0) {
	if (_coefTermConsumer != 0) {
	  _generatedCoefTermConsumer.reset
		(new TranslatingCoefTermConsumer(_coefTermConsumer, _translator));
	} else {
	  ASSERT(_ioHandler != 0);
	  ASSERT(_out != 0);
	  _generatedCoefTermConsumer.reset
		(_ioHandler->createPolynomialWriter(_translator, _out));
	}

	if (_canonicalOutput)
	  _generatedCoefTermConsumer.reset
		(new CanonicalCoefTermConsumer
		 (_generatedCoefTermConsumer, _ideal->getVarCount()));
  }

  return _generatedCoefTermConsumer.get();
}
