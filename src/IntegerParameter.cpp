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
#include "IntegerParameter.h"

#include "error.h"
#include "FrobbyStringStream.h"

IntegerParameter::IntegerParameter(const char* name, const char* description,
				   unsigned int defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

const char* IntegerParameter::getParameterName() const {
  return "INTEGER";
}

void IntegerParameter::getValue(string& str) const {
  str.clear();
  FrobbyStringStream::appendIntegerToString(str, _value);
}

unsigned int IntegerParameter::getIntegerValue() const {
  return _value;
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

  mpz_class integer;
  try {
	FrobbyStringStream::parseInteger(integer, params[0]);
  } catch (const FrobbyStringStream::NotAnIntegerException&) {
	FrobbyStringStream errorMsg;
	errorMsg << "Option -"
			 << getName()
			 << " was given the parameter \""
			 << params[0]
			 << "\", and the only valid parameters are integers "
			 << "between 0 and 2^32-1.";
	reportError(errorMsg);
  }

  if (!integer.fits_uint_p()) {
	FrobbyStringStream errorMsg;
	errorMsg << "Option -"
			 << getName()
			 << " was given the parameter "
			 << params[0]
			 << ", and this is outside the allowed range [0, 2^32-1].";
	reportError(errorMsg);
  }

  _value = integer.get_ui();
}
