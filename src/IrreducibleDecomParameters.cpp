#include "stdinc.h"
#include "IrreducibleDecomParameters.h"

IrreducibleDecomParameters::IrreducibleDecomParameters():
  ParameterGroup("", ""),
  
  _doBenchmark
  ("bench",
   "Compute the irreducible decomposition without producing output.",
   false),
  
  _printDebug
  ("debug",
   "Report what the algorithm does. Useful for debugging.",
   false),
  
  _printProgress
  ("progress",
   "Periodically print how far the computation is along.",
   false),
  
  _printStatistics
  ("stats",
   "Print statistics on what the algorithm did.",
   false),
  
  _skipRedundant
  ("skipRedundant",
   "Eliminate some redundant computations. This is not always a win.",
   true),
  
    _useBound
  ("bound",
   "Use a bound to eliminate some useless computations when solving an IDP.",
     false),
  
  _useIndependence
  ("independence",
   "Perform independence splits when possible.",
   true),
  
  _useSlice
  ("slice",
   "Use the Slice Algorithm in place of the Label Algorithm.",
   true),

  _split
  ("split",
   "The split selection strategy to use. Options are label, minart, midart and maxart. (slice only)",
 "midart") {
  addParameter(&_doBenchmark);
  addParameter(&_printDebug);
  addParameter(&_printProgress);
  addParameter(&_printStatistics);
  addParameter(&_skipRedundant);
  addParameter(&_useBound);
  addParameter(&_useIndependence);
  addParameter(&_useSlice);
  addParameter(&_split);
}

void IrreducibleDecomParameters::setUseIndependence(bool value) {
  _useIndependence = value;
}

bool IrreducibleDecomParameters::getDoBenchmark() const {
  return _doBenchmark;
}

bool IrreducibleDecomParameters::getPrintDebug() const {
  return _printDebug;
}

bool IrreducibleDecomParameters::getPrintProgress() const {
  return _printProgress;
}

bool IrreducibleDecomParameters::getPrintStatistics() const {
  return _printStatistics;
}

bool IrreducibleDecomParameters::getSkipRedundant() const {
  return _skipRedundant;
}

bool IrreducibleDecomParameters::getUseBound() const {
  return _useBound;
}

bool IrreducibleDecomParameters::getUseIndependence() const {
  return _useIndependence;
}

bool IrreducibleDecomParameters::getUseSlice() const {
  return _useSlice;
}

const string& IrreducibleDecomParameters::getSplit() const {
  return _split;
}
