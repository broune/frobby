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

class StringParameter : public Parameter {
public:
  StringParameter(const char* name,
				  const char* description,
				  const string& defaultValue);

  virtual const char* getParameterName() const;

  virtual void getValue(string& str) const;
  const string& getValue() const;

  operator const string&() const;

  StringParameter& operator=(const string& value);

  virtual void processParameters(const char** params, unsigned int paramCount);

private:
  string _value;
};

#endif
