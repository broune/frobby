#include "stdinc.h"
#include "GenerateIdealParameters.h"

GenerateIdealParameters::GenerateIdealParameters():
  ParameterGroup("", ""),
  _exponentRange
  ("expRange",
   "Exponents are chosen uniformly in the range [0,INTEGER].", 9),

  _variableCount("varCount", "The number of variables.", 3),
  _generatorCount("genCount", "The number of minimal generators.", 5) {
  addParameter(&_exponentRange);
  addParameter(&_variableCount);
  addParameter(&_generatorCount);
}

unsigned int GenerateIdealParameters::getExponentRange() const {
  return _exponentRange;
}

unsigned int GenerateIdealParameters::getVariableCount() const {
  return _variableCount;
}

unsigned int GenerateIdealParameters::getGeneratorCount() const {
  return _generatorCount;
}
