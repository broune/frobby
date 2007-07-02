#include "stdinc.h"
#include "ParameterGroup.h"

ParameterGroup::ParameterGroup(const char* name, const char* description):
  Parameter(name, description) {
}

ParameterGroup::~ParameterGroup() {
}

void ParameterGroup::getValue(string& str) const {
  str = "...";
}

bool ParameterGroup::process(const char** params, unsigned int paramCount) {
  for (vector<Parameter*>::const_iterator it = _parameters.begin();
       it != _parameters.end(); ++it)
    if ((*it)->process(params, paramCount))
      return true;

  return false;
}

void ParameterGroup::processParameters(const char** params,
				       unsigned int paramCount) {
  ASSERT(false);
  cerr << "INTERNAL ERROR: Called ParameterGroup::processParameters" << endl;
  exit(1);
}

void ParameterGroup::printState(ostream& out) {
  for (vector<Parameter*>::const_iterator it = _parameters.begin();
       it != _parameters.end(); ++it)
    (*it)->printState(out);
}

void ParameterGroup::addParameter(Parameter* parameter) {
  ASSERT(parameter != 0);

  _parameters.push_back(parameter);
}

void ParameterGroup::obtainParameters(vector<Parameter*>& parameters) {
  parameters.insert(parameters.begin(),
		    _parameters.begin(),
		    _parameters.end());
}
