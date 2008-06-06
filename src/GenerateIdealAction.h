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
#ifndef GENERATOR_IDEAL_ACTION_GUARD
#define GENERATOR_IDEAL_ACTION_GUARD

#include "Action.h"
#include "StringParameter.h"
#include "IntegerParameter.h"
#include "IOParameters.h"

class GenerateIdealAction : public Action {
 public:
  GenerateIdealAction();

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

  static const char* staticGetName();

 private:
  StringParameter _type;

  IntegerParameter _variableCount;
  IntegerParameter _generatorCount;
  IntegerParameter _exponentRange;

  IOParameters _io;
};

#endif
