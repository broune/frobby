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
#include "error.h"
#include "FrobbyStringStream.h"

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
  if ('-' + string(_name) != params[0])
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

  FrobbyStringStream errorMsg;

  errorMsg << "Option -" << getName() << " takes ";
  if (from == to) {
    if (from == 1)
	  errorMsg << "one parameter, ";
    else
      errorMsg << from << " parameters, ";
  } else
	errorMsg << "from " << from << " to " << to << " parameters, ";

  if (paramCount == 0)
	errorMsg << "but no parameters were provided.\n";
  else {
    if (paramCount == 1)
      errorMsg << "but one parameter was provided.";
    else
	  errorMsg << "but " << paramCount << " parameters were provided.";
	errorMsg << '\n';

	errorMsg << "The provided parameters were: ";
    const char* prefix = "";
    for (unsigned int i = 0; i < paramCount; ++i) {
	  errorMsg << prefix << params[i];
      prefix = ", ";
    }
	errorMsg << ".\n";
	
    if (paramCount > to)
	  errorMsg <<
		"(Did you forget to put a - in front of one of the options?)\n";
  }
  
  errorMsg << "The option -"
		   << getName()
		   << " has the following description:\n "
		   << getDescription();

  reportError(errorMsg);
}
