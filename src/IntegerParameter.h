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
#ifndef INTEGER_PARAMETER_GUARD
#define INTEGER_PARAMETER_GUARD

#include "Parameter.h"
#include <utility>
#include <string>

class IntegerParameter : public Parameter {
public:
  IntegerParameter(const string& name,
                   const string& description,
                   unsigned int defaultValue);

  unsigned int getValue() const {return _value;}
  void setValue(unsigned int value) {_value = value;}

  operator unsigned int() const {return getValue();}
  void operator=(unsigned int value) {setValue(value);}

private:
  virtual string doGetArgumentType() const;
  virtual string doGetValueAsString() const;
  virtual pair<size_t, size_t> doGetArgumentCountRange() const;
  virtual void doProcessArguments(const char** args, size_t argCount);

  unsigned int _value;
};

#endif
