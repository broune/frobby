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
#ifndef STRING_PARAMETER_GUARD
#define STRING_PARAMETER_GUARD

#include "Parameter.h"
#include <utility>
#include <string>

class StringParameter : public Parameter {
public:
  StringParameter(const string& name,
                  const string& description,
                  const string& defaultValue);

  const string& getValue() const {return _value;}
  void setValue(const string& value) {_value = value;}

  operator const string&() const {return getValue();}
  void operator=(const string& value) {setValue(value);}
  bool operator==(const string& str) const {return getValue() == str;}

private:
  virtual string doGetArgumentType() const;
  virtual string doGetValueAsString() const;
  virtual pair<size_t, size_t> doGetArgumentCountRange() const;
  virtual void doProcessArguments(const char** args, size_t argCount);

  string _value;
};

#endif
