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
#ifndef SLICE_PARAMETERS_GUARD
#define SLICE_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "BoolParameter.h"
#include "StringParameter.h"

class SliceFacade;

/** This class represents the parameters associated to the Slice
 Algorithm. This allows actions to share these options without code
 duplication.

 Most of these parameters are encapsulated and cannot be accessed,
 except that SliceParameters is able to set up a SliceFacade according
 to the parameters it encapsulates. */
class SliceParameters : public ParameterGroup {
 public:
  /** Construct the parameters with default values.

   @param exposeBoundParams Make available parameters related to the
     bound optimization.
   @param exposeIndependenceParam Make available the parameter to turn
     independence splits on or off.
  */
  SliceParameters(bool exposeBoundParams = false,
				  bool exposeIndependenceParam = true);

  /** Set facade up according to the values of the options of this
   object. */
  void apply(SliceFacade& facade) const;

  /** Set the value of the independence split option. This is useful
   for setting the default value before parsing the user-supplied
   options.
  */
  void setUseIndependence(bool value);

  /** Set the value of the option for choosing the split selection
   strategy. This is useful for setting the default value before
   parsing the user-supplied options.
  */
  void setSplit(const string& split);

  /** Checks that the split selection strategy specified is
   valid. Reports an error otherwise. */
  void validateSplit(bool allowLabel, bool allowDegree);

  /** Get the value of the bound elimination option. This value has to
   be exposed as it is not part of the state of a SliceFacade.
  */
  bool getUseBoundElimination() const;

  /** Get the value of the bound simplification option. This value has
   to be exposed as it is not part of the state of a SliceFacade.
  */
  bool getUseBoundSimplification() const;

 private:
  bool _exposeBoundParam;
  bool _exposeIndependenceParam;

  BoolParameter _printDebug;
  BoolParameter _printStatistics;
  BoolParameter _useBoundElimination;
  BoolParameter _useBoundSimplification;
  BoolParameter _useIndependence;
  BoolParameter _useSimplification;
  BoolParameter _minimal;
  BoolParameter _canonical;

  StringParameter _split;
};

#endif
