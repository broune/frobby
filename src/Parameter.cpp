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

  fprintf(stderr, "ERROR: Option -%s takes ", getName());
  if (from == to) {
    if (from == 1)
      fputs("one parameter, ", stderr);
    else
      fprintf(stderr, "%u parameters, ", from);
  } else
    fprintf(stderr, "from %u to %u parameters, ", from, to);

  if (paramCount == 0)
    fputs("but no parameters were provided.\n", stderr);
  else {
    if (paramCount == 1)
      fputs("but one parameter was provided.\n", stderr);
    else
      fprintf(stderr, "but %u parameters were provided.\n", paramCount);

    fputs("The provided parameters were: ", stderr);
    const char* prefix = "\"";
    for (unsigned int i = 0; i < paramCount; ++i) {
      fprintf(stderr, "%s%s\"", prefix, params[i]);
      prefix = ", \"";
    }
    fputc('\n', stderr);

    if (paramCount > to)
      fputs("(Did you forget to put a - in front of one of the options?)\n",
	    stderr);
  }
    
  fprintf(stderr, "\nThe option -%s has the following description:\n%s\n",
	  getName(), _description);
  exit(1);
}
