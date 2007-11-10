#ifndef STRING_PARAMETER_GUARD
#define STRING_PARAMETER_GUARD

#include "Parameter.h"

class StringParameter : public Parameter {
public:
  StringParameter(const char* name,
		  const char* description,
		  const string& defaultValue);

  virtual const char* getParameterName() const;

  virtual void getValue(string& str) const;
  const string& getValue() const;

  operator const string&() const;

  StringParameter& operator=(const string& value);

  virtual void processParameters(const char** params, unsigned int paramCount);

  virtual void printState(ostream& out);

private:
  string _value;
};

#endif
