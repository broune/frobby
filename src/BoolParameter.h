#ifndef BOOL_PARAMETER_GUARD
#define BOOL_PARAMETER_GUARD

#include "Parameter.h"

class BoolParameter : public Parameter {
public:
  BoolParameter(const char* name, const char* description, bool defaultValue);

  virtual const char* getParameterName() const;
  virtual void getValue(string& str) const;
  virtual void processParameters(const char** params, unsigned int paramCount);

  operator bool() const;
  BoolParameter& operator=(bool value);

private:
  bool _value;
};

#endif
