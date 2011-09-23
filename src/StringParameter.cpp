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
#include "StringParameter.h"

StringParameter::StringParameter(const string& name,
                                 const string& description,
                                 const string& defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

string StringParameter::doGetArgumentType() const {
  return "STRING";
}

string StringParameter::doGetValueAsString() const {
  return _value;
}

pair<size_t, size_t> StringParameter::doGetArgumentCountRange() const {
  return make_pair(1, 1);
}

void StringParameter::doProcessArguments(const char** args, size_t argCount) {
  ASSERT(argCount == 1);
  _value = args[0];
}
