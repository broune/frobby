#ifndef STRING_PARAMETER_GUARD
#define STRING_PARAMETER_GUARD

#include "Parameter.h"

class StringParameter : public Parameter {
public:
  StringParameter(const char* name,
		  const char* description,
		  const string& defaultValue):
    Parameter(name, description),
    _value(defaultValue) {
  }

  virtual ~StringParameter() {
  }

  virtual const char* getParameterName() const {
    return "STRING";
  }

  virtual void getValue(string& str) const {
    str = _value;
  }

  operator const string&() const {
    return _value;
  }

  StringParameter& operator=(const string& value) {
    _value = value;
    return *this;
  }

  void processParameters(const char** params, unsigned int paramCount) {
    checkCorrectParameterCount(1, 1, params, paramCount);
    ASSERT(paramCount == 1);

    _value = params[0];
  }

  virtual void printState(ostream& out) {
    out << getName() << " = \"" << _value << "\"";
  }

private:
  string _value;
};

#endif
