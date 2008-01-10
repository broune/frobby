#ifndef IRREDUCIBLE_DECOM_PARAMETERS_GUARD
#define IRREDUCIBLE_DECOM_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "BoolParameter.h"
#include "StringParameter.h"

class IrreducibleDecomParameters : public ParameterGroup {
 public:
  IrreducibleDecomParameters();

  void setUseIndependence(bool value);
  void setSplit(const string& split);

  bool getDoBenchmark() const;
  bool getPrintDebug() const;
  bool getPrintProgress() const;
  bool getPrintStatistics() const;
  bool getSkipRedundant() const;
  bool getUseBound() const;
  bool getUseIndependence() const;
  bool getUseSlice() const;
  
  const string& getSplit() const;

 private:
  BoolParameter _doBenchmark;
  BoolParameter _printDebug;
  BoolParameter _printProgress;
  BoolParameter _printStatistics;
  BoolParameter _skipRedundant;
  BoolParameter _useBound;
  BoolParameter _useIndependence;
  BoolParameter _useSlice;

  StringParameter _split;
};

#endif
