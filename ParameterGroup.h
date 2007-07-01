#ifndef PARAMETER_GROUP_GUARD
#define PARAMETER_GROUP_GUARD

#include "Parameter.h"

class ParameterGroup : public Parameter {
 public:
  ParameterGroup(const char* name, const char* description);
  virtual ~ParameterGroup();

  virtual bool process(const char** params, unsigned int paramCount);
  virtual void processParameters(const char** params, unsigned int paramCount);
  virtual void printState(ostream& out);

  void addParameter(Parameter* parameter);

  void obtainParameters(vector<Parameter*>& parameters);

  virtual void getValue(string& str) const;

 private:
  vector<Parameter*> _parameters;
};

#endif
