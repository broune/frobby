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

#include "TranslatingTermConsumer.h"

IrreducibleDecomFacade::
IrreducibleDecomFacade(bool printActions,
					   const IrreducibleDecomParameters& parameters):
  Facade(printActions),
  _parameters(parameters) {
}
#include <iostream>
void IrreducibleDecomFacade::
printLabels(BigIdeal& bigIdeal, FILE* out, const string& format) {
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);

  Ideal idealCopy(ideal);
  Ideal decom(ideal.getVarCount());

  if (idealCopy.getGeneratorCount() > 0)
	translator.addArtinianPowers(idealCopy);

  computeIrreducibleDecom(idealCopy, new DecomRecorder(&decom));
  // idealCopy gets cleared here, which is why we make a copy.

  beginAction("Computing labels");

  IdealWriter* writer =
	IOHandler::getIOHandler(format)->createWriter(out, &translator);

  size_t varCount = decom.getVarCount();
  Ideal::const_iterator decomEnd = decom.end();
  for (Ideal::const_iterator d = decom.begin(); d != decomEnd; ++d) {
	Term de(*d, varCount);
	fputc('*', out);
	writer->writeJustATerm(de);
	fputc('\n', out);
	for (size_t var = 0; var < varCount; ++var)
	  if (de[var] == 0)
		de[var] = translator.getMaxId(var);

	vector<vector<const Exponent*> > labels(varCount + 1);
	Ideal::const_iterator end = ideal.end();
	for (Ideal::const_iterator it = ideal.begin(); it != end; ++it) {
	  if (de.dominates(*it)) {
		size_t equalVar = 0;
		size_t equalCount = 0;
		for (size_t var = 0; var < varCount; ++var) {
		  if (de[var] == (*it)[var]) {
			equalVar = var;
			++equalCount;
		  }
		}
		if (equalCount == 1)
		  labels[equalVar].push_back(*it);
		else
		  labels[varCount].push_back(*it);
	  }
	}

	for (size_t var = 0; var <= varCount; ++var) {
	  if (var == varCount && labels[var].empty())
		continue;

	  if (var < varCount)
		fprintf(out, " %s:", translator.getNames().getName(var).c_str());
	  else
		fputs(" other:", out);

	  if (var < varCount && de[var] == translator.getMaxId(var))
		fputs(" unbounded", out);

	  for (size_t i = 0; i < labels[var].size(); ++i) {
		if (i != 0)
		  fputc(',', out);
		writer->writeJustATerm(Term(labels[var][i], varCount));
	  }
	  fputc('\n', out);
	}
	fputc('\n', out);
  }

  endAction();
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(Ideal& ideal, TermConsumer* consumer) {
  computeIrreducibleDecom(ideal, consumer, false);
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(Ideal& ideal,
						TermConsumer* consumer,
						bool preMinimized) {
  if (!_parameters.getMinimal() && !preMinimized) {
	beginAction("Minimizing input ideal.");
	ideal.minimize();
	endAction();
  }

  beginAction("Computing irreducible decomposition.");

  if (ideal.contains(Term(ideal.getVarCount()))) {
	consumer->consume(Term(ideal.getVarCount()));
	delete consumer;
  } else {
    MsmStrategy* strategy =
      MsmStrategy::newDecomStrategy(_parameters.getSplit(), consumer);    
    runSliceAlgorithm(ideal, strategy);
  }

  endAction();
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(BigIdeal& bigIdeal,
						BigTermConsumer* consumerParameter) {
  beginAction("Preparing to compute irreducible decomposition.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();

  if (ideal.getGeneratorCount() > 0)
    translator.addArtinianPowers(ideal);

  TermConsumer* consumer =
	new TranslatingTermConsumer(consumerParameter, &translator);

  endAction();

  computeIrreducibleDecom(ideal, consumer, false);  
}
/* TODO: remove
void IrreducibleDecomFacade::
computeIrreducibleDecom(BigIdeal& bigIdeal, FILE* out, const string& format) {
  beginAction("Preparing to compute irreducible decomposition.");

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();

  if (ideal.getGeneratorCount() > 0)
    translator.addArtinianPowers(ideal);

  TermConsumer* consumer =
	IOHandler::getIOHandler(format)->createWriter(out, &translator);

  endAction();

  computeIrreducibleDecom(ideal, consumer, false);
}
*/

void IrreducibleDecomFacade::
computeAlexanderDual(BigIdeal& bigIdeal,
					 const vector<mpz_class>* pointParameter,
					 BigTermConsumer* consumerParameter) {
  ASSERT(pointParameter == 0 ||
		 pointParameter->size() == bigIdeal.getVarCount());
  // We have to remove the non-minimal generators before we take the
  // lcm, since the Alexander dual works on the lcm of only the
  // minimal generators.
  if (!_parameters.getMinimal()) {
	IdealFacade facade(isPrintingActions());
	facade.sortAllAndMinimize(bigIdeal);
  }

  beginAction("Preparing to compute Alexander dual.");

  vector<mpz_class> lcm(bigIdeal.getVarCount());
  bigIdeal.getLcm(lcm);

  vector<mpz_class> point;

  if (pointParameter == 0)
	point = lcm;
  else {
	point = *pointParameter;
	for (size_t var = 0; var < bigIdeal.getVarCount(); ++var) {
	  if (point[var] < lcm[var]) {
		fputs("ERROR: The specified point to dualize on is not divisible by the\n"
			  "least common multiple of the minimal generators of the ideal.\n", stderr);
		exit(1);
	  }
	}
  }

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  translator.dualize(point);
  bigIdeal.clear();

  if (ideal.getGeneratorCount() > 0)
    translator.addArtinianPowers(ideal);

  TermConsumer* consumer =
	new TranslatingTermConsumer(consumerParameter, &translator);

  endAction();

  computeIrreducibleDecom(ideal, consumer, true);
}

void IrreducibleDecomFacade::
computeFrobeniusNumber(const vector<mpz_class>& instance,
					   BigIdeal& bigIdeal, 
					   mpz_class& frobeniusNumber,
					   vector<mpz_class>& bigVector) {
  if (instance.size() == 2) {
    frobeniusNumber = instance[0] * instance[1] - instance[0] - instance[1];
	bigVector.resize(2);
	bigVector[0] = -1;
	bigVector[1] = instance[0] - 1;
    return;
  }
  
  beginAction("Preparing to compute Frobenius number.");
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  bigIdeal.clear();
  translator.addArtinianPowers(ideal);
  endAction();

  if (!_parameters.getMinimal()) {
	beginAction("Minimizing input ideal.");
	ideal.minimize();
	endAction();
  }

  beginAction("Optimizing over irreducible decomposition.");

  Ideal maxSolution(ideal.getVarCount());
  DecomRecorder recorder(&maxSolution);
  vector<mpz_class> shiftedDegrees(instance.begin() + 1, instance.end());
  TermGrader grader(shiftedDegrees, &translator);

  MsmStrategy* strategy = FrobeniusStrategy::newFrobeniusStrategy
	(_parameters.getSplit(), &recorder, grader, _parameters.getUseBound());

  runSliceAlgorithm(ideal, strategy);

  ASSERT(maxSolution.getGeneratorCount() == 1);
  Term solution(*maxSolution.begin(), maxSolution.getVarCount());
  grader.getDegree(solution, frobeniusNumber);
  bigVector.resize(instance.size());
  bigVector[0] = -1;
  for (size_t i = 0; i < instance.size(); ++i) {
	frobeniusNumber -= instance[i];
	if (i > 0)
	  bigVector[i] = translator.getExponent(i - 1, solution) - 1;
  }
  
  endAction();
}

void IrreducibleDecomFacade::
runSliceAlgorithm(Ideal& ideal, SliceStrategy* strategy) {
  ASSERT(strategy != 0);

  strategy->setUseIndependence(_parameters.getUseIndependence());

  if (_parameters.getPrintDebug())
	strategy = new DebugStrategy(strategy, stderr);

  // TODO: reimplement
  //if (_parameters.getPrintStatistics())
  //  strategyParam = MsmStrategy::addStatistics(strategyParam);

  ::computeMaximalStandardMonomials(ideal, strategy);
  delete strategy; // TODO: is this way of doing it really necessary?
}
