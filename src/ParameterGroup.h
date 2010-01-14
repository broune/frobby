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
#ifndef PARAMETER_GROUP_GUARD
#define PARAMETER_GROUP_GUARD

#include "Parameter.h"
#include <vector>

class ParameterGroup {
 public:
  typedef vector<Parameter*>::iterator iterator;

  iterator begin() {return _parameters.begin();}
  iterator end() {return _parameters.end();}

  ParameterGroup();
  virtual ~ParameterGroup();

  void addParameter(Parameter* parameter);

  void obtainParameters(vector<Parameter*>& parameters);

  virtual void getValue(string& str) const;

 private:
  vector<Parameter*> _parameters;
};

#endif
