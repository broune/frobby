/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
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
  return _name.c_str();
}

const char* Parameter::getDescription() const {
  return _description.c_str();
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
		  getName(), getDescription());
  exit(1);
}
