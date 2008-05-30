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
#include "AnalyzeAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"
#include "IrreducibleDecomParameters.h"
#include "Scanner.h"

#include <algorithm>

AnalyzeAction::AnalyzeAction():
  _printLcm
  ("lcm",
   "Print the least common multiple of the generators.",
   false),
  
  _printVarCount
  ("varCount",
   "Print the number of variables.",
   false),

  _printGeneratorCount
  ("genCount",
   "Print the number of generators.",
   false),

  _printMaximumExponent
  ("maxExp",
   "Print the largest exponent that appears in the input file",
   false),

  _printMinimal
  ("minimal",
   "Print 1 if the ideal has no non-minimal generators. Print 0 otherwise.",
   false) {
}

const char* AnalyzeAction::getName() const {
  return "analyze";
}

const char* AnalyzeAction::getShortDescription() const {
  return "Display information about the input ideal.";
}

const char* AnalyzeAction::getDescription() const {
  return
"Display information about input ideal. This is useful for getting a quick\n"
"impression of how the ideal looks like, and it can be used in scripts\n"
"that need information about the ideal.";
}

Action* AnalyzeAction::createNew() const {
  return new AnalyzeAction();
}

void AnalyzeAction::obtainParameters(vector<Parameter*>& parameters) {
  Action::obtainParameters(parameters);
  _io.obtainParameters(parameters);

  parameters.push_back(&_printLcm);
  parameters.push_back(&_printVarCount);
  parameters.push_back(&_printGeneratorCount);
  parameters.push_back(&_printMaximumExponent);
  parameters.push_back(&_printMinimal);
}

void AnalyzeAction::perform() {
  Scanner in(_io.getInputFormat(), stdin);
  _io.autoDetectInputFormat(in);
  _io.validateFormats();

  BigIdeal ideal;

  IOFacade ioFacade(_printActions);
  ioFacade.readIdeal(in, ideal);

  IdealFacade idealFacade(_printActions);
  if (_printLcm)
	idealFacade.printLcm(stdout, ideal);
  if (_printVarCount) {
	fprintf(stdout, "%lu\n", (unsigned long)ideal.getVarCount());
  }
  if (_printGeneratorCount) {
	fprintf(stdout, "%lu\n", (unsigned long)ideal.getGeneratorCount());	
  }
  if (_printMaximumExponent) {
	if (ideal.getVarCount() == 0)
	  fputs("0\n", stdout);
	else {
	  vector<mpz_class> lcm;
	  ideal.getLcm(lcm);
	  gmp_fprintf(stdout, "%Zd\n",
				  max_element(lcm.begin(), lcm.end())->get_mpz_t());
	}
  }
  if (_printMinimal) {
	BigIdeal clone(ideal);
	idealFacade.sortAllAndMinimize(clone);
	if (ideal.getGeneratorCount() == clone.getGeneratorCount())
	  fputs("1\n", stdout);
	else
	  fputs("0\n", stdout);
  }

  idealFacade.printAnalysis(stderr, ideal);
}
