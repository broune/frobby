/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 Bjarke Hammersholt Roune (www.broune.com)

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
#include "EulerAction.h"

#include "DataType.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "BigIdeal.h"
#include "BigTermConsumer.h"
#include "error.h"
#include "PivotEulerAlg.h"
#include "Ideal.h"
#include "HilbertBaseCase.h"

#include <algorithm>
#include <cstdio>

/** @todo Expand the description of this action. */
EulerAction::EulerAction():
  Action
(staticGetName(),
 "Compute the Euler characteristic of a simplicial complex.",
 "Compute the Euler characteristic of a simplicial complex.",
 false),

  _algorithm
  ("algorithm",
   "Which algorithm to use. Options are auto, pivot, mv and hybrid.",
   "hybrid"),

  _printStatistics
  ("stats",
   "Print statistics on what the algorithm did.",
   false),

  _useUniqueDivSimplify
  ("oneEle",
   "Simplify complexes where an element is contained in only one facet.",
   true),

  _useManyDivSimplify
  ("manyEle",
   "Simplify complexes where an element is contained in almost all facets.",
   true),

  _useAllPairsSimplify
  ("impliedEle",
   "Simplify complexes with elements x and y such that all facets that "
   "contain x also contain y.",
   false),

  _io(DataType::getMonomialIdealType(), DataType::getNullType()) {
}

void EulerAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_algorithm);
  parameters.push_back(&_printStatistics);
  parameters.push_back(&_useUniqueDivSimplify);
  parameters.push_back(&_useManyDivSimplify);
  parameters.push_back(&_useAllPairsSimplify);
  Action::obtainParameters(parameters);
}

void EulerAction::perform() {
  BigIdeal ideal;
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(in, ideal);
  in.expectEOF();

  size_t varCount = ideal.getVarCount();
  size_t genCount = ideal.getGeneratorCount();

  Ideal radical(varCount);
  Term tmp(varCount);
  for (size_t term = 0; term < genCount; ++term) {
    for (size_t var = 0; var < varCount; ++var) {
      if (ideal[term][var] == 0)
        tmp[var] = 0;
      else if (ideal[term][var] == 1)
        tmp[var] = 1;
	  else
		reportError("Input ideal is not square free.");
    }
    radical.insert(tmp);
  }

  radical.minimize();

  mpz_class euler;
  if (false) {
	HilbertBasecase basecase;
	basecase.computeCoefficient(radical);
	euler = basecase.getLastCoefficient();
	gmp_fprintf(stdout, "%Zd\n", euler.get_mpz_t());
	return;
  }

  PivotEulerAlg alg;
  if (_algorithm.getValue() == "hybrid")
	alg.setAlgorithm(PivotEulerAlg::HybridAlg);
  else if (_algorithm.getValue() == "pivot")
	alg.setAlgorithm(PivotEulerAlg::PivotAlg);
  else if (_algorithm.getValue() == "mv")
	alg.setAlgorithm(PivotEulerAlg::MayerVietorisAlg);
  else
    reportError("Unknown Euler characteristic algorithm \"" +
                _algorithm.getValue() + "\".");

  alg.setPrintStatistics(_printStatistics, stderr);
  alg.setUseUniqueDivSimplify(_useUniqueDivSimplify);
  alg.setUseManyDivSimplify(_useManyDivSimplify);
  alg.setUseAllPairsSimplify(_useAllPairsSimplify);

  euler = alg.computeEulerCharacteristic(radical);
  gmp_fprintf(stdout, "%Zd\n", euler.get_mpz_t());  
}

const char* EulerAction::staticGetName() {
  return "euler";
}
