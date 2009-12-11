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
#ifndef GENERATOR_FROBENIUS_ACTION_GUARD
#define GENERATOR_FROBENIUS_ACTION_GUARD

#include "Action.h"
#include "IntegerParameter.h"

class GenerateFrobeniusAction : public Action {
 public:
  GenerateFrobeniusAction();

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

  static const char* staticGetName();

 private:
  IntegerParameter _entryCount;
  IntegerParameter _maxEntryDigits;
};

#endif
