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

IntegerParameter::IntegerParameter(const string& name,
                                   const string& description,
                                   unsigned int defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

string IntegerParameter::doGetArgumentType() const {
  return "INTEGER";
}

string IntegerParameter::doGetValueAsString() const {
  FrobbyStringStream str;
  str << getValue();
  return str.str();
}

pair<size_t, size_t> IntegerParameter::doGetArgumentCountRange() const {
  return make_pair(1, 1);
}

void IntegerParameter::doProcessArguments(const char** args, size_t argCount) {
  ASSERT(argCount == 1);

  bool ok = true;
  mpz_class integer;
  try {
    FrobbyStringStream::parseInteger(integer, args[0]);
  } catch (const FrobbyStringStream::NotAnIntegerException&) {
    ok = false;
  }

  if (!ok || !integer.fits_uint_p()) {
    FrobbyStringStream errorMsg;
    errorMsg << "Option -"
             << getName()
             << " was given the parameter \""
             << args[0]
             << "\", which is not an integer in the range [0, 2^31-1].";
    reportError(errorMsg);
  }

  _value = integer.get_ui();
}
