#ifndef IRREDUCIBLE_DECOM_PARAMETERS_GUARD
#define IRREDUCIBLE_DECOM_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "BoolParameter.h"

class IrreducibleDecomParameters : public ParameterGroup {
 public:
  IrreducibleDecomParameters();

  void setSkipRedundant(bool value);

  bool getDoBenchmark() const;
  bool getPrintDebug() const;
  bool getPrintProgress() const;
  bool getPrintStatistics() const;
  bool getSkipRedundant() const;
  bool getUseBound() const;
  bool getUsePartition() const;

 private:
  BoolParameter _doBenchmark;
  BoolParameter _printDebug;
  BoolParameter _printProgress;
  BoolParameter _printStatistics;
  BoolParameter _skipRedundant;
  BoolParameter _useBound;
  BoolParameter _usePartition;
};

#endif
