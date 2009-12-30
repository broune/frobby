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
#include "BoolParameter.h"

#include "error.h"
#include "FrobbyStringStream.h"

BoolParameter::BoolParameter(const char* name,
			     const char* description,
			     bool defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

const char* BoolParameter::getParameterName() const {
  return "[BOOL]";
}

void BoolParameter::getValue(string& str) const {
  if (_value)
    str = "on";
  else
    str = "off";
}

BoolParameter::operator bool() const {
  return _value;
}

BoolParameter& BoolParameter::operator=(bool value) {
  _value = value;
  return *this;
}

void BoolParameter::
processParameters(const char** params, unsigned int paramCount) {
  checkCorrectParameterCount(0, 1, params, paramCount);
  
  if (paramCount == 0) {
    _value = true;
    return;
  }
  ASSERT(paramCount == 1);

  string param(params[0]);
  if (param == "off")
    _value = false;
  else if (param == "on")
    _value = true;
  else {
	FrobbyStringStream errorMsg;
	errorMsg << "Option -"
			 << getName()
			 << " was given the parameter \""
			 << param
			 << "\". The only valid parameters are \"on\" and \"off\".";
	reportError(errorMsg);
  }
}
