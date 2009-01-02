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

class IntegerParameter : public Parameter {
public:
  IntegerParameter(const char* name, const char* description,
				   unsigned int defaultValue);

  virtual const char* getParameterName() const;

  virtual void getValue(string& str) const;
  unsigned int getIntegerValue() const;

  operator unsigned int() const;

  IntegerParameter& operator=(unsigned int value);

  void processParameters(const char** params, unsigned int paramCount);

private:
  unsigned int _value;
};

#endif
