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
#ifndef HILBERT_ACTION_GUARD
#define HILBERT_ACTION_GUARD

#include "Action.h"
#include "IOParameters.h"
#include "BoolParameter.h"

class HilbertAction : public Action {
 public:
  HilbertAction();

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

  static const char* staticGetName();

 private:
  IOParameters _io;
  BoolParameter _univariate;
  BoolParameter _canonical;
};

#endif
