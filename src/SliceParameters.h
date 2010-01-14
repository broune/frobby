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
class BigattiFacade;

/** This class represents the parameters associated to the Slice
 Algorithm and the similar Bigatti et.al. Hilbert-Poincare series
 algorithm. This allows actions to share these options without code
 duplication. Note that SliceParameters is able to set up a
 SliceFacade and BigattiFacade according to the parameters it
 encapsulates.
*/
class SliceParameters : public ParameterGroup {
 public:
  /** Construct the parameters with default values.

   @param exposeBoundParams Make available parameters related to the
     bound optimization.
   @param exposeIndependenceParam Make available the parameter to turn
     independence splits on or off.
   @param supportBigattiAlgorithm Adjust messages
  */
  SliceParameters(bool exposeBoundParams = false,
                  bool exposeIndependenceParam = true,
                  bool supportBigattiAlgorithm = false);

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

  /** Checks that the slice split selection strategy specified is
   valid. Reports an error otherwise. */
  void validateSplit(bool allowLabel, bool allowDegree);

  /** Checks that the Bigatti Et.al. pivot selection strategy specified is
   valid. Reports an error otherwise. */
  void validateSplitHilbert();

  /** Get the value of the bound elimination option. This value has to
   be exposed as it is not part of the state of a SliceFacade.
  */
  bool getUseBoundElimination() const;

  /** Get the value of the bound simplification option. This value has
   to be exposed as it is not part of the state of a SliceFacade.
  */
  bool getUseBoundSimplification() const;

  /** Get the value of the canonical option. */
  bool getCanonical() const;

  /** Get the value of the split option. */
  const string& getSplit() const;

  /** Returns the value of the debug option. */
  bool getPrintDebug() const;

  /** Returns the value of the stats option. */
  bool getPrintStatistics() const;

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
  BoolParameter _useBigattiGeneric;
  BoolParameter _widenPivot;

  StringParameter _split;
};

#endif
