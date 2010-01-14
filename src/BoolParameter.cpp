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

BoolParameter::BoolParameter(const string& name,
                             const string& description,
                             bool defaultValue):
  Parameter(name, description),
  _value(defaultValue) {
}

string BoolParameter::doGetArgumentType() const {
  return "[BOOL]";
}

string BoolParameter::doGetValueAsString() const {
  if (_value)
    return "on";
  else
    return "off";
}

pair<size_t, size_t> BoolParameter::doGetArgumentCountRange() const {
  return make_pair(0, 1);
}

void BoolParameter::doProcessArguments(const char** args, size_t argCount) {
  if (argCount == 0) {
    _value = true;
    return;
  }
  ASSERT(argCount == 1);

  string arg(args[0]);
  if (arg == "off")
    _value = false;
  else if (arg == "on")
    _value = true;
  else {
    FrobbyStringStream errorMsg;
    errorMsg << "Option -"
             << getName()
             << " was given the argument \""
             << arg
             << "\". The only valid arguments are \"on\" and \"off\".";
    reportError(errorMsg);
  }
}
