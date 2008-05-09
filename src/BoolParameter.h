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
#ifndef BOOL_PARAMETER_GUARD
#define BOOL_PARAMETER_GUARD

#include "Parameter.h"

class BoolParameter : public Parameter {
public:
  BoolParameter(const char* name, const char* description, bool defaultValue);

  virtual const char* getParameterName() const;
  virtual void getValue(string& str) const;
  virtual void processParameters(const char** params, unsigned int paramCount);

  operator bool() const;
  BoolParameter& operator=(bool value);

private:
  bool _value;
};

#endif
