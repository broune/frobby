#ifndef GENERATE_IDEAL_PARAMETERS_GUARD
#define GENERATE_IDEAL_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "IntegerParameter.h"

class GenerateIdealParameters : public ParameterGroup {
 public:
  GenerateIdealParameters();

  unsigned int getExponentRange() const;
  unsigned int getVariableCount() const;
  unsigned int getGeneratorCount() const;
  
 private:
  IntegerParameter _exponentRange;
  IntegerParameter _variableCount;
  IntegerParameter _generatorCount;
};

#endif
