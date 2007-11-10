#include "stdinc.h"
#include "StringParameter.h"

StringParameter::StringParameter(const char* name,
				 const char* description,
				 const string& defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

const char* StringParameter::getParameterName() const {
  return "STRING";
}

void StringParameter::getValue(string& str) const {
  str = _value;
}

const string& StringParameter::getValue() const {
  return _value;
}

StringParameter::operator const string&() const {
  return _value;
}

StringParameter& StringParameter::operator=(const string& value) {
  _value = value;
  return *this;
}

void StringParameter::processParameters(const char** params,
					unsigned int paramCount) {
  checkCorrectParameterCount(1, 1, params, paramCount);
  ASSERT(paramCount == 1);
  
  _value = params[0];
}

void StringParameter::printState(ostream& out) {
  out << getName() << " = \"" << _value << "\"";
}
