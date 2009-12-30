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
#include "BigattiFacade.h"

#include "BigIdeal.h"
#include "IOHandler.h"
#include "VarSorter.h"
#include "TranslatingCoefTermConsumer.h"
#include "TotalDegreeCoefTermConsumer.h"
#include "BigattiHilbertAlgorithm.h"
#include "BigattiParams.h"
#include "IOFacade.h"
#include "IOParameters.h"
#include "DataType.h"
#include "Scanner.h"

BigattiFacade::BigattiFacade
(const BigIdeal& bigIdeal, IOHandler* handler, FILE* out,
 bool printActions):
  Facade(printActions),
  _ideal(bigIdeal.getVarCount()),
  _ioHandler(handler),
  _out(out) {
  ASSERT(_ioHandler != 0);

  beginAction("Translating ideal to internal data structure.");
  _translator.reset(new TermTranslator(bigIdeal, _ideal, false));
  endAction();
}

BigattiFacade::BigattiFacade(const BigattiParams& params):
  Facade(params.getPrintActions()),
  _out(stdout) {

  setDoCanonicalOutput(params.getProduceCanonicalOutput());
  setIsMinimallyGenerated(params.getIdealIsMinimal());
  setPrintStatistics(params.getPrintStatistics());
  setPrintDebug(params.getPrintDebug());
  setUseGenericBaseCase(params.getUseGenericBaseCase());
  setUseSimplification(params.getUseSimplification());

  setPivotStrategy
	(BigattiPivotStrategy::createStrategy(params.getPivot(),
										  params.getWidenPivot()));

  BigIdeal bigIdeal;
  {
	IOParameters io(DataType::getMonomialIdealType(),
					DataType::getPolynomialType());
	Scanner in(params.getInputFormat(), stdin);
	io.setInputFormat(params.getInputFormat());
	io.setOutputFormat(params.getOutputFormat());
	io.autoDetectInputFormat(in);
	io.validateFormats();

	// TODO: fix leak
	_ioHandler = IOHandler::createIOHandler
	  (io.getOutputFormat()).release(); 

	IOFacade facade(params.getPrintActions());
	facade.readIdeal(in, bigIdeal);
	in.expectEOF();
  }

  beginAction("Translating ideal to internal data structure.");
  _translator.reset(new TermTranslator(bigIdeal, _ideal, false));
  endAction();
}

void BigattiFacade::setPrintStatistics(bool value) {
  _printStatistics = value;
}

void BigattiFacade::setPrintDebug(bool value) {
  _printDebug = value;
}

void BigattiFacade::setUseGenericBaseCase(bool value) {
  _useGenericBaseCase = value;
}

void BigattiFacade::setPivotStrategy(auto_ptr<BigattiPivotStrategy> pivot) {
  _pivot = pivot;
}

void BigattiFacade::setIsMinimallyGenerated(bool value) {
  _isMinimallyGenerated = value;
}

void BigattiFacade::setDoCanonicalOutput(bool value) {
  _doCanonicalOutput = value;
}

void BigattiFacade::setUseSimplification(bool value) {
  _useSimplification = value;
}

void BigattiFacade::minimize() {
  if (_isMinimallyGenerated)
	return;

  beginAction("Minimizing ideal.");

  _ideal.minimize();
  _isMinimallyGenerated = true;

  endAction();
}

void BigattiFacade::computeMultigradedHilbertSeries() {
  runAlgorithm("Computing multigraded Hilbert-Poincare series.", false);
}

void BigattiFacade::computeUnivariateHilbertSeries() {
  runAlgorithm("Computing univariate Hilbert-Poincare series", true);
}

void BigattiFacade::sortVars() {
  beginAction("Sorting variables for canonical representation.");

  VarSorter sorter(_translator->getNames());
  sorter.permute(_translator.get());

  Ideal::iterator stop = _ideal.end();
  for (Ideal::iterator it = _ideal.begin(); it != stop; ++it)
    sorter.permute(*it);

  endAction();
}

void BigattiFacade::runAlgorithm
(const char* action, bool univariate) {
  minimize();
  if (_doCanonicalOutput)
	sortVars();

  auto_ptr<CoefBigTermConsumer> consumer =
	_ioHandler->createPolynomialWriter(_out);

  consumer->consumeRing(_translator->getNames());

  beginAction(action);
  BigattiHilbertAlgorithm alg(_ideal, *_translator, *consumer);
  alg.setPrintStatistics(_printStatistics);
  alg.setPrintDebug(_printDebug);
  alg.setUseGenericBaseCase(_useGenericBaseCase);
  alg.setPivotStrategy(_pivot);
  alg.setUseSimplification(_useSimplification);
  alg.setDoCanonicalOutput(_doCanonicalOutput);
  alg.setComputeUnivariate(univariate);

  alg.run();

  endAction();
}
