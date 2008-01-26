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
  
  _printStatistics
  ("stats",
   "Print statistics on what the algorithm did.",
   false),
  
  _skipRedundant
  ("skipRedundant",
   "Eliminate redundant computations and output. (label only)",
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

  _minimal
  ("minimal",
   "Specifies that the input ideal is minimally generated by the given generators. Turning this on can improve performance, but if it is not true, the results will be unpredictable.",
   false),

  _split
  ("split",
   "The split selection strategy to use. Options are maxlabel, minlabel,\n"
   "varlabel, minimum, median, maximum, mingen, indep and gcd. Frobenius\n"
   "computations support the specialized strategy frob as well. (slice only)",
   "median") {
  addParameter(&_minimal);
  addParameter(&_split);
  addParameter(&_doBenchmark);
  addParameter(&_printStatistics);
  addParameter(&_useSlice);
  addParameter(&_useIndependence);
  addParameter(&_printDebug);
  addParameter(&_skipRedundant);
  addParameter(&_useBound);
}

void IrreducibleDecomParameters::setUseIndependence(bool value) {
  _useIndependence = value;
}

void IrreducibleDecomParameters::setSplit(const string& split) {
  _split = split;
}

bool IrreducibleDecomParameters::getDoBenchmark() const {
  return _doBenchmark;
}

bool IrreducibleDecomParameters::getPrintDebug() const {
  return _printDebug;
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

bool IrreducibleDecomParameters::getMinimal() const {
  return _minimal;
}

const string& IrreducibleDecomParameters::getSplit() const {
  return _split;
}
