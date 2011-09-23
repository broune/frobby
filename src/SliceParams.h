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
#ifndef SLICE_PARAMS_GUARD
#define SLICE_PARAMS_GUARD

#include "SliceLikeParams.h"
#include <string>
class CliParams;

class SliceParams : public SliceLikeParams {
 public:
  SliceParams();
  SliceParams(const CliParams& params);

  const string& getSplit() const {return _split;}
  void setSplit(const string& name) {_split = name;}

  bool getUseIndependenceSplits() const {return _useIndependence;}
  void useIndependenceSplits(bool value) {_useIndependence = value;}

  /** Returns whether to use branch-and-bound to speed up Slice
   optimization computations by eliminating non-improving slices. */
  bool getUseBoundElimination() const {return _useBoundElimination;}
  void useBoundElimination(bool value) {_useBoundElimination = value;}

  /** Returns whether to simplify slices by seeking to generate
   non-improving slices that are then eliminated. This requires
   that elimination of non-improving slices is turned on. */
  bool getUseBoundSimplification() const {return _useBoundSimplification;}
  void useBoundSimplification(bool value) {_useBoundSimplification = value;}

 private:
  string _split;
  bool _useIndependence;
  bool _useBoundElimination;
  bool _useBoundSimplification;
};

void addIdealParams(CliParams& params);
void extractCliValues(SliceParams& slice, const CliParams& cli);

void validateSplit(const SliceParams& params,
                   bool allowLabel,
                   bool allowDegree);

#endif
