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
#include "IrreducibleDecomFacade.h"

#include "IOHandler.h"
#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "VarNames.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "SliceAlgorithm.h"
#include "MsmStrategy.h"
#include "TermIgnorer.h"
#include "DecomRecorder.h"
#include "TermGrader.h"
#include "TermConsumer.h"
#include "IdealFacade.h"
#include "DebugStrategy.h"
#include "FrobeniusStrategy.h"
#include "BigTermRecorder.h"

#include "TranslatingTermConsumer.h"

#include "SliceFacade.h"

IrreducibleDecomFacade::
IrreducibleDecomFacade(bool printActions,
					   const IrreducibleDecomParameters& parameters):
  Facade(printActions),
  _parameters(parameters) {
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(BigIdeal& bigIdeal,
						BigTermConsumer* consumerParameter) {
  SliceFacade facade(bigIdeal, consumerParameter, isPrintingActions());
  _parameters.apply(facade);
  facade.computeIrreducibleDecomposition();
}

void IrreducibleDecomFacade::
computeFrobeniusNumber(const vector<mpz_class>& degrees,
					   BigIdeal& bigIdeal, 
					   vector<mpz_class>& bigVector) {
  BigIdeal maxSolution(bigIdeal.getVarCount());
  BigTermRecorder consumer(&maxSolution);

  SliceFacade facade(bigIdeal, &consumer, isPrintingActions());
  _parameters.apply(facade);
  facade.solveStandardProgram(degrees, _parameters.getUseBound());

  ASSERT(maxSolution.getGeneratorCount() == 1);
  bigVector = maxSolution[0];
}
