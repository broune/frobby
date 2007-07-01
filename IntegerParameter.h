#ifndef INTEGER_PARAMETER_GUARD
#define INTEGER_PARAMETER_GUARD

#include "Parameter.h"

class IntegerParameter : public Parameter {
public:
  IntegerParameter(const char* name, const char* description,
		   unsigned int defaultValue);

  virtual const char* getParameterName() const;

  virtual void getValue(string& str) const;

  operator unsigned int() const;

  IntegerParameter& operator=(unsigned int value);

  void processParameters(const char** params, unsigned int paramCount);
  virtual void printState(ostream& out);

private:
  unsigned int _value;
};

#endif
