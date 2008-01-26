#include "stdinc.h"
#include "BoolParameter.h"

BoolParameter::BoolParameter(const char* name,
			     const char* description,
			     bool defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

const char* BoolParameter::getParameterName() const {
  return "[BOOL]";
}

void BoolParameter::getValue(string& str) const {
  if (_value)
    str = "on";
  else
    str = "off";
}

BoolParameter::operator bool() const {
  return _value;
}

BoolParameter& BoolParameter::operator=(bool value) {
  _value = value;
  return *this;
}

void BoolParameter::
processParameters(const char** params, unsigned int paramCount) {
  checkCorrectParameterCount(0, 1, params, paramCount);
  
  if (paramCount == 0) {
    _value = true;
    return;
  }
  ASSERT(paramCount == 1);

  string param(params[0]);
  if (param == "off")
    _value = false;
  else if (param == "on")
    _value = true;
  else {
    fprintf(stderr, "ERROR: Option -%s was given the parameter \"%s\".\n"
			"The only valid parameters are \"on\" and \"off\".\n",
			getName(), param.c_str());
    exit(1);
  }
}
