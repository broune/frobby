#include "stdinc.h"
#include "IntegerParameter.h"

#include <sstream>

IntegerParameter::IntegerParameter(const char* name, const char* description,
				   unsigned int defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

const char* IntegerParameter::getParameterName() const {
  return "INTEGER";
}

void IntegerParameter::getValue(string& str) const {
  ostringstream out;
  out << _value;
  str = out.str();
}

IntegerParameter::operator unsigned int() const {
  return _value;
}

IntegerParameter& IntegerParameter::operator=(unsigned int value) {
  _value = value;
  return *this;
}

void IntegerParameter::processParameters
(const char** params, unsigned int paramCount) {
  checkCorrectParameterCount(1, 1, params, paramCount);
  ASSERT(paramCount == 1);
    
  const char* param = params[0];
  if (param[0] == '+' ||
      param[0] == '-')
    ++param;

  for (const char* it = param; *it != '\0'; ++it) {
    if (!isdigit(*it)) {
      cerr << "ERROR: Option -" << getName()
	   << " was given the parameter \""
	   << params[0] << "\".\n"
	   << "The only valid parameters are integers between 0 and 2^32-1." << endl;
      exit(1);
    }
  }

  stringstream in(params[0]);
  mpz_class integer;
  in >> integer;

  if (!integer.fits_uint_p()) {
    cerr << "ERROR: Option -" << getName()
	 << " was given the parameter " << params[0]
	 << ".\nThis is outside the allowed range between 0 and 2^32-1."
	 << endl;
    exit(1);
  }

  _value = integer.get_ui();
}

void IntegerParameter::printState(ostream& out) {
  out << getName() << " = " << _value;
}
