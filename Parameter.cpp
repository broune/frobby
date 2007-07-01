#include "stdinc.h"
#include "Parameter.h"

Parameter::Parameter(const char* name,
		     const char* description):
  _name(name),
  _description(description) {
}

Parameter::~Parameter() {
}

const char* Parameter::getName() const {
  return _name;
}

const char* Parameter::getDescription() const {
  return _description;
}

const char* Parameter::getParameterName() const {
  return "";
}

bool Parameter::process(const char** params, unsigned int paramCount) {
  if (string(_name) != params[0])
    return false;

  processParameters(params + 1, paramCount - 1);
  return true;
}

void Parameter::checkCorrectParameterCount(unsigned int from,
					   unsigned int to,
					   const char** params,
					   unsigned int paramCount) {
  if (from <= paramCount && paramCount <= to)
    return;

  cerr << "ERROR: Option -" << getName() << " takes ";
  if (from == to)
    cerr << from << " parameter, ";
  else
    cerr << "from " << from << " to " << to << " parameters, ";

  if (paramCount == 0)
    cerr << "but no parameters were provided." << endl;
  else {
    if (paramCount == 1)
      cerr << "but one parameter was provided." << endl;
    else
      cerr << "but " << paramCount
	   << " parameters were provided." << endl;

    cerr << "The provided parameters were: ";
    const char* prefix = "\"";
    for (unsigned int i = 0; i < paramCount; ++i) {
      cerr << prefix << params[i] << '"';
      prefix = ", \"";
    }
    cerr << endl;

    if (paramCount > to)
      cerr << "(Did you forget to put a - in front of one of the options?)" << endl;
  }
    
  cerr << endl
       << "The option -" << getName() << " has the following description:"
       << endl << _description << endl;
  exit(0);
}
