/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef SQUARE_FREE_IDEAL_GUARD
#define SQUARE_FREE_IDEAL_GUARD

#include "VarNames.h"
class RawSquareFreeIdeal;
class BigIdeal;

class SquareFreeIdeal {
 public:
  SquareFreeIdeal();
  SquareFreeIdeal(const BigIdeal& ideal);

  /** Takes over ownership of ideal. */
  SquareFreeIdeal(const VarNames& names, RawSquareFreeIdeal* ideal);
  ~SquareFreeIdeal();

  SquareFreeIdeal& operator=(const BigIdeal& ideal);

  void minimize();

  void clear();
  void swap(SquareFreeIdeal& ideal);

  const RawSquareFreeIdeal* getRawIdeal() const {return _ideal;}
  RawSquareFreeIdeal* getRawIdeal() {return _ideal;}

 private:
  SquareFreeIdeal(const SquareFreeIdeal&); // not available
  SquareFreeIdeal& operator=(const SquareFreeIdeal&); // not available

  VarNames _names;
  RawSquareFreeIdeal* _ideal;
};

#endif
