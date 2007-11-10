#include "stdinc.h"
#include "IrreducibleDecomFacade.h"

#include "label/PrintDebugStrategy.h"
#include "label/FrobeniusStrategy.h"
#include "label/BenchmarkStrategy.h"
#include "label/DecompositionStrategy.h"
#include "label/StatisticsStrategy.h"
#include "label/CompositeStrategy.h"
#include "label/PrintProgressStrategy.h"
#include "label/SkipRedundantStrategy.h"
#include "label/LabelAlgorithm.h"

#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "VarNames.h"
#include "TermTranslator.h"
#include "TermList.h"
#include "SliceAlgorithm.h"
#include "DecomWriter.h"
#include "SliceStrategy.h"
#include "DebugDecomConsumer.h"

IrreducibleDecomFacade::
IrreducibleDecomFacade(bool printActions,
		       const IrreducibleDecomParameters& parameters):
  Facade(printActions),
  _parameters(parameters) {
}

void IrreducibleDecomFacade::
computeIrreducibleDecom(Ideal* ideal, DecomConsumer* decomConsumer) {
  beginAction("Computing irreducible decomposition.");

  SliceStrategy* strategy =
    SliceStrategy::newStrategy(_parameters.getSplit());
  if (strategy == 0) {
    cerr << "ERROR: Unknown split strategy \""
	 << _parameters.getSplit()
	 << "\"." << endl;
    exit(1);
  }

  runSliceAlgorithm(ideal, decomConsumer, strategy);

  endAction();
}

// TODO: make more widespread use of DecomConsumer.
void IrreducibleDecomFacade::
computeIrreducibleDecom(BigIdeal& ideal, ostream& out) {
  beginAction("Computing irreducible decomposition.");

  ASSERT(ideal.getNames().getVarCount() >= 1);
  
  // Special case for when the ideal is all of the ring, i.e. minimally
  // generated by the monomial 1. Also special case for when there is only
  // one variable.
  if (ideal.getVarCount() == 1 ||
      (ideal.getGeneratorCount() == 1 &&
       ideal[0] == vector<mpz_class>(ideal.getNames().getVarCount()))) {
    IOFacade ioFacade(false);
    ioFacade.writeIdeal(out, ideal);
    return;
  }

  if (_parameters.getUseSlice()) {
    Ideal* terms = 0;
    TermTranslator* translator = 0;
    ideal.buildAndClear(terms, translator, false);

    SliceStrategy* strategy =
      SliceStrategy::newStrategy(_parameters.getSplit());
    if (strategy == 0) {
      cerr << "ERROR: Unknown split strategy \""
	   << _parameters.getSplit()
	   << "\"." << endl;
      exit(1);
    }
    
    runSliceAlgorithm(terms,
		      new DecomWriter(ideal.getNames(), translator, out),
		      strategy);
    delete translator;
  } else {
    Ideal* terms = 0;
    TermTranslator* translator = 0;
    ideal.buildAndClear(terms, translator, false);

    Strategy* strategy;
    if (_parameters.getDoBenchmark())
      strategy = new BenchmarkStrategy();
    else
      strategy = new DecompositionStrategy
	(&out, ideal.getNames(), ideal.getNames().getVarCount(), translator);
    runLabelAlgorithm(terms, translator, strategy);
  }

  endAction();
}

void IrreducibleDecomFacade::
computeFrobeniusNumber(const vector<mpz_class>& instance,
		       BigIdeal& ideal, 
		       mpz_class& frobeniusNumber) {
  beginAction
    ("Optimizing over irreducible decomposition using label algorithm.");

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

  Ideal* terms;
  TermTranslator* translator;
  ideal.buildAndClear(terms, translator, false);

  Strategy* strategy = new FrobeniusStrategy
    (instance, &frobeniusNumber, ideal.getNames().getVarCount(),
     translator, _parameters.getUseBound());
  runLabelAlgorithm(terms, translator, strategy);

  endAction();
}

void IrreducibleDecomFacade::
runSliceAlgorithm(Ideal* ideal, DecomConsumer* consumer,
		  SliceStrategy* strategy) {
  ASSERT(strategy != 0);
  ASSERT(ideal != 0);
  ASSERT(consumer != 0);

  SliceAlgorithm alg;

  if (_parameters.getPrintStatistics())
    strategy = SliceStrategy::addStatistics(strategy);
  if (_parameters.getPrintDebug()) {
    strategy = SliceStrategy::addDebugOutput(strategy);
    consumer = new DebugDecomConsumer(consumer);
  }

  alg.setUseIndependence(_parameters.getUseIndependence());

  alg.setConsumer(consumer);
  alg.setStrategy(strategy);
  alg.runAndDeleteIdealAndReset(ideal);
}


// All parameters are deleted.
void IrreducibleDecomFacade::
runLabelAlgorithm(Ideal* ideal, TermTranslator* translator,
		  Strategy* strategy) {
  ASSERT(ideal != 0);
  ASSERT(translator != 0);
  ASSERT(strategy != 0);

  // Set up the combined strategy
  if (_parameters.getPrintProgress())
    strategy = new CompositeStrategy(strategy, new PrintProgressStrategy());

  if (_parameters.getPrintStatistics())
    strategy = new CompositeStrategy
      (strategy, new StatisticsStrategy(ideal->getVarCount()));

  if (_parameters.getPrintDebug())
    strategy = new CompositeStrategy(strategy, new PrintDebugStrategy());

  if (_parameters.getSkipRedundant()) {
    strategy = new SkipRedundantStrategy(strategy, ideal->getVarCount());
  }
  
  // Run algorithm
  LabelAlgorithm algo;
  algo.setStrategy(strategy);
  algo.setUsePartition(_parameters.getUseIndependence());
  algo.runAndDeleteIdealAndReset(ideal);

  delete translator;
}
