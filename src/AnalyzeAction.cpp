#include "stdinc.h"
#include "AnalyzeAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"
#include "IrreducibleDecomParameters.h"
#include "IrreducibleDecomFacade.h"
#include "Scanner.h"

#include <algorithm>

AnalyzeAction::AnalyzeAction():
  _printLcm
  ("lcm",
   "Print the least common multiple of the generators.",
   false),
  
  _printLabels
  ("label",
   "Print the irreducible decomposition along with labels.",
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
  parameters.push_back(&_printLabels);
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
  if (_printLabels) {
	IrreducibleDecomParameters params;
	IrreducibleDecomFacade irrFacade(_printActions, params);
	irrFacade.printLabels(ideal, stdout, _io.getOutputFormat());
  }
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
