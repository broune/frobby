#ifndef IRREDUCIBLE_DECOM_PARAMETERS_GUARD
#define IRREDUCIBLE_DECOM_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "BoolParameter.h"
#include "StringParameter.h"

class IrreducibleDecomParameters : public ParameterGroup {
 public:
  IrreducibleDecomParameters(bool exposeBoundParam = false);

  void setUseIndependence(bool value);
  void setSplit(const string& split);

  bool getPrintDebug() const;
  bool getPrintStatistics() const;
  bool getUseBound() const;
  bool getUseIndependence() const;
  bool getMinimal() const;
  
  const string& getSplit() const;

 private:
  bool _exposeBoundParam;

  BoolParameter _printDebug;
  BoolParameter _printStatistics;
  BoolParameter _useBound;
  BoolParameter _useIndependence;
  BoolParameter _minimal;

  StringParameter _split;
};

#endif
