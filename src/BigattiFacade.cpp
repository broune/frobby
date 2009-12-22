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
  auto_ptr<CoefBigTermConsumer> bigConsumer =
	_ioHandler->createPolynomialWriter(_out);
  TranslatingCoefTermConsumer consumer(*bigConsumer, *_translator);
  runAlgorithm("Computing multigraded Hilbert-Poincare series.", consumer);
}

void BigattiFacade::computeUnivariateHilbertSeries() {
  auto_ptr<CoefBigTermConsumer> bigConsumer =
	_ioHandler->createPolynomialWriter(_out);
  TotalDegreeCoefTermConsumer consumer(*bigConsumer, *_translator);
  runAlgorithm("Computing univariate Hilbert-Poincare series", consumer);
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

void BigattiFacade::
runAlgorithm(const char* action, CoefTermConsumer& consumer) {
  minimize();
  if (_doCanonicalOutput)
	sortVars();

  consumer.consumeRing(_translator->getNames());

  beginAction(action);
  BigattiHilbertAlgorithm alg(_ideal, consumer);
  alg.setPrintStatistics(_printStatistics);
  alg.setPrintDebug(_printDebug);
  alg.setUseGenericBaseCase(_useGenericBaseCase);
  alg.setPivotStrategy(_pivot);
  alg.setUseSimplification(_useSimplification);
  alg.setDoCanonicalOutput(_doCanonicalOutput);

  alg.run();

  endAction();
}
