/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#ifndef BIGATTI_PARAMS_GUARD
#define BIGATTI_PARAMS_GUARD

#include "SliceLikeParams.h"

class CliParams;

class BigattiParams : public SliceLikeParams {
 public:
  BigattiParams(const CliParams& cli);

  /** Returns whether to detect generic monomial ideals as a base
      case. */
  bool getUseGenericBaseCase() const {return _useGenericBaseCase;}
  void setPivot(const string& name) {_pivot = name;}

  /** Returns the name of the pivot selection strategy used for
      pivots splits. */
  const string& getPivot() const {return _pivot;}
  void useGenericBaseCase(bool value) {_useGenericBaseCase = value;}

  /** Returns whether to widen the pivots of pivot splits. */
  bool getWidenPivot() const {return _widenPivot;}
  void widenPivot(bool value) {_widenPivot = value;}

 private:
  string _pivot;
  bool _widenPivot;
  bool _useGenericBaseCase;
};

void addBigattiParams(CliParams& params);
void extractCliValues(BigattiParams& slice, const CliParams& cli);

#endif
