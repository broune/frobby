#include "stdinc.h"
#include "IrreducibleDecomFacade.h"

#include "label/FrobeniusStrategy.h"
#include "label/BenchmarkStrategy.h"
#include "label/DecompositionStrategy.h"
#include "label/LabelAlgorithm.h"

#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "VarNames.h"
#include "TermTranslator.h"
#include "Ideal.h"
#include "SliceAlgorithm.h"
#include "DecomWriter.h"
#include "SliceStrategy.h"
#include "DecomIgnorer.h"
#include "DecomRecorder.h"
#include "TermGrader.h"

IrreducibleDecomFacade::
IrreducibleDecomFacade(bool printActions,
		       const IrreducibleDecomParameters& parameters):
  Facade(printActions),
  _parameters(parameters) {
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(Ideal& ideal, DecomConsumer* decomConsumer) {
  beginAction("Computing irreducible decomposition.");

  if (_parameters.getUseSlice()) {
    SliceStrategy* strategy =
      SliceStrategy::newDecomStrategy(_parameters.getSplit(), decomConsumer);
    if (strategy == 0) {
      cerr << "ERROR: Unknown split strategy \""
	   << _parameters.getSplit()
	   << "\"." << endl;
      exit(1);
    }
    
    runSliceAlgorithm(ideal, strategy);
  } else {
    Strategy* strategy =
      new DecompositionStrategy(decomConsumer, ideal.getVarCount());
    runLabelAlgorithm(ideal, strategy);
  }

  endAction();
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(BigIdeal& bigIdeal, FILE* out) {
  beginAction("Preparing to compute irreducible decomposition.");

  ASSERT(bigIdeal.getVarCount() >= 1);

  // Special case for when the ideal is all of the ring, i.e. minimally
  // generated by the monomial 1. Also special case for when there is only
  // one variable.
  if (bigIdeal.getVarCount() == 1 ||
      (bigIdeal.getGeneratorCount() == 1 &&
      bigIdeal[0] == vector<mpz_class>(bigIdeal.getVarCount()))) {
    IOFacade ioFacade(false);
    ioFacade.writeIdeal(out, bigIdeal);
    return;
  }

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();
  translator.addArtinianPowers(ideal);

  DecomConsumer* decomConsumer;
  if (_parameters.getDoBenchmark())
    decomConsumer = new DecomIgnorer();
  else
    decomConsumer = new DecomWriter(translator.getNames(), &translator, out);
  
  endAction();

  computeIrreducibleDecom(ideal, decomConsumer);
}

void IrreducibleDecomFacade::
computeFrobeniusNumber(const vector<mpz_class>& instance,
		       BigIdeal& bigIdeal, 
		       mpz_class& frobeniusNumber) {
  beginAction("Optimizing over irreducible decomposition.");

  if (_parameters.getSkipRedundant()) {
    cerr << "ERROR: Due to implementation issues, the Grobner basis" << endl
	 << "based Frobenius feature does not support independence splits."
	 << endl;
    exit(1);
  }

  if (instance.size() == 2) {
    frobeniusNumber = instance[0] * instance[1] - instance[0] - instance[1];
    return;
  }

  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal, false);
  bigIdeal.clear();
  translator.addArtinianPowers(ideal);

  if (_parameters.getUseSlice()) {
    Ideal i(ideal.getVarCount());
    DecomRecorder recorder(&i);
    vector<mpz_class> shiftedDegrees(instance.begin() + 1, instance.end());
    TermGrader grader(shiftedDegrees, &translator);
    
    SliceStrategy* strategy = SliceStrategy::newFrobeniusStrategy
      (_parameters.getSplit(), &recorder, grader);
    
     // TODO: factor out common error code into SliceStrategy.
     if (strategy == 0) {
       cerr << "ERROR: Unknown split strategy \""
	    << _parameters.getSplit()
	    << "\"." << endl;
       exit(1);
     }

     runSliceAlgorithm(ideal, strategy);

     ASSERT(i.getGeneratorCount() == 1);
     grader.getDegree(Term(*i.begin(), i.getVarCount()), frobeniusNumber);
     for (size_t i = 0; i < instance.size(); ++i)
       frobeniusNumber -= instance[i];
  } else {
    Strategy* strategy = new FrobeniusStrategy
      (instance, &frobeniusNumber, ideal.getVarCount(),
       &translator, _parameters.getUseBound());
    runLabelAlgorithm(ideal, strategy);
  }

  endAction();
}

void IrreducibleDecomFacade::
runSliceAlgorithm(Ideal& ideal, SliceStrategy* strategy) {
  ASSERT(strategy != 0);

  if (_parameters.getPrintStatistics())
    strategy = SliceStrategy::addStatistics(strategy);

  if (_parameters.getPrintDebug())
    strategy = SliceStrategy::addDebugOutput(strategy);

  SliceAlgorithm alg;
  alg.setUseIndependence(_parameters.getUseIndependence());
  alg.setStrategy(strategy);
  alg.runAndClear(ideal);
}

void IrreducibleDecomFacade::
runLabelAlgorithm(Ideal& ideal, Strategy* strategy) {
  ASSERT(strategy != 0);

  if (_parameters.getPrintProgress())
    strategy = Strategy::addDebugOutput(strategy);

  if (_parameters.getPrintStatistics())
    strategy = Strategy::addStatistics(strategy, ideal.getVarCount());

  if (_parameters.getPrintDebug())
    strategy = Strategy::addDebugOutput(strategy);

  if (_parameters.getSkipRedundant())
    strategy = Strategy::addSkipRedundant(strategy, ideal.getVarCount());

  // Run algorithm
  LabelAlgorithm algo;
  algo.setStrategy(strategy);
  algo.setUsePartition(_parameters.getUseIndependence());
  algo.runAndClear(ideal);
}
