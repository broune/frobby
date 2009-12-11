/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef LATTICE_ANALYZE_ACTION_GUARD
#define LATTICE_ANALYZE_ACTION_GUARD

#include "Action.h"
#include "BoolParameter.h"
#include "IOParameters.h"
#include "IntegerParameter.h"

class LatticeAnalyzeAction : public Action {
 public:
  LatticeAnalyzeAction();

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

  static const char* staticGetName();

  /// This action isn't done yet, so it should not be displayed by
  /// the help action.
  virtual bool displayAction() const;

 private:
  IOParameters _io;
};

#endif
