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
#include "stdinc.h"
#include "SliceParameters.h"

#include "SliceFacade.h"
#include "error.h"
#include "BigattiPivotStrategy.h"
#include "BigattiFacade.h"

SliceParameters::SliceParameters(bool exposeBoundParam,
								 bool exposeIndependenceParam,
								 bool supportBigattiAlgorithm):
  _exposeBoundParam(exposeBoundParam),
  _exposeIndependenceParam(exposeIndependenceParam),

  _printDebug
  ("debug",
   "Print what the algorithm does at each step.",
   false),

  _printStatistics
  ("stats",
   "Print statistics on what the algorithm did.",
   false),

  _useBoundElimination
  ("bound",
   "Use the bound optimization to solve optimization problems faster\n"
   "by eliminating non-improving slices.",
   exposeBoundParam),

  _useBoundSimplification
  ("boundSimplify",
   "Use the bound optimization to solve optimization problems faster by\n"
   "simplifying slices through seeking to generate non-improving slices.\n"
   "This requires turning elimination of non-improving slices on.",
   exposeBoundParam),

  _useIndependence
  ("independence",
   "Perform independence splits when possible.",
   exposeIndependenceParam),

  _useSimplification
  ("simplify",
   "Perform simplification when possible.",
   true),

  _minimal
  ("minimal",
   "Specifies that the input ideal is minimally generated by the given\n"
   "generators. Turning this on can improve performance, but if it is not\n"
   "true then Frobby may go into an infinite loop or produce incorrect results.",
   false),

  _canonical
  ("canon",
   "Sort the output, including the variables, to get a canonical\n"
   "representation. This requires storing the entire output in memory, which\n"
   "can increase run time modestly and increase memory consumption greatly.",
   false),

  _useBigattiGeneric
  ("genericBase",
   "Detect generic ideals as a base case of the Bigatti algorithm.",
   true),

  _split
  ("split",
   "The split selection strategy to use. Slice options are maxlabel, minlabel,\n"
   "varlabel, minimum, median, maximum, mingen, indep and gcd. Optimization\n"
   "computations support the specialized strategy degree as well.",
   "median") {
  addParameter(&_minimal);
  addParameter(&_split);
  addParameter(&_printStatistics);
  if (exposeIndependenceParam)
	addParameter(&_useIndependence);
  addParameter(&_useSimplification);
  addParameter(&_printDebug);
  if (_exposeBoundParam) {
	addParameter(&_useBoundSimplification);
	addParameter(&_useBoundElimination);
  }
  addParameter(&_canonical);

  if (supportBigattiAlgorithm) {
	addParameter(&_useBigattiGeneric);

	_printDebug.appendToDescription
	  (" Slice algorithm only.");
	_printStatistics.appendToDescription
	  (" Slice algorithm only.");
	_useIndependence.appendToDescription
	  (" Slice algorithm only.");
	_minimal.appendToDescription
	  ("\nSlice algorithm only.");
	_canonical.appendToDescription
	  ("\nThe impact for the Bigatti et.al. algorithm is much less than for the"
	   "\nSlice Algorithm since the Bigatti et.al. algorithm always has to\n"
	   "store the entire output in memory regardless of this option.");
	_split.appendToDescription
	  ("\nBigatti et.al. options are median, mostNGPure, mostNGGcd,\n"
	   "mostNGTight, typicalPure, typicalGcd, typicalTight, typicalNGPure,\n"
	   "typicalNGGcd, typicalNGTight, someNGPure, someNGGcd and someNGTight.");
  }
}

void SliceParameters::setUseIndependence(bool value) {
  _useIndependence = value;
}

void SliceParameters::setSplit(const string& split) {
  _split = split;
}

bool SliceParameters::getUseBoundElimination() const {
  return _useBoundElimination;
}

bool SliceParameters::getUseBoundSimplification() const {
  return _useBoundSimplification;
}

bool SliceParameters::getCanonical() const {
  return _canonical;
}

const string& SliceParameters::getSplit() const {
  return _split.getValue();
}

void SliceParameters::validateSplit(bool allowLabel,
									bool allowDegree) {
  auto_ptr<SplitStrategy>
	split(SplitStrategy::createStrategy(_split.getValue().c_str()));

  if (split.get() == 0)
	reportError("Unknown Slice split strategy \"" + _split.getValue() + "\".");

  if (!allowLabel && split->isLabelSplit())
	reportError("Label split strategy is not appropriate "
				"in this context.");

  // TODO: implement degree when there is no grading too, so that it
  // is always appropriate.
  if (!allowDegree && _split.getValue() == "degree") {
	reportError("The split strategy degree is not appropriate "
				"in this context.");
  }

  // TODO: remove the deprecated frob.
  if (!allowDegree && _split.getValue() == "frob") {
	reportError("The split strategy frob is not appropriate "
				"in this context.");
  }
}

void SliceParameters::validateSplitHilbert() {
  auto_ptr<BigattiPivotStrategy>
	split(BigattiPivotStrategy::createStrategy(_split.getValue().c_str()));

  if (split.get() == 0)
	reportError("Unknown Bigatti et.al. pivot strategy \"" +
				_split.getValue() + "\".");
}

void SliceParameters::apply(SliceFacade& facade) const {
  auto_ptr<SplitStrategy> split =
	SplitStrategy::createStrategy(_split.getValue().c_str());
  facade.setSplitStrategy(split);

  facade.setPrintDebug(_printDebug);
  facade.setPrintStatistics(_printStatistics);
  facade.setUseIndependence(_useIndependence);
  facade.setUseSimplification(_useSimplification);
  facade.setIsMinimallyGenerated(_minimal);
  if (_canonical)
	facade.setCanonicalOutput();
}

void SliceParameters::apply(BigattiFacade& facade) const {
  auto_ptr<BigattiPivotStrategy> pivot =
	BigattiPivotStrategy::createStrategy(_split.getValue().c_str());
  facade.setPivotStrategy(pivot);

  facade.setPrintDebug(_printDebug);
  facade.setPrintStatistics(_printStatistics);
  facade.setUseSimplification(_useSimplification);
  facade.setIsMinimallyGenerated(_minimal);
  facade.setDoCanonicalOutput(_canonical);
  facade.setUseGenericBaseCase(_useBigattiGeneric);
}

bool SliceParameters::getPrintDebug() const {
  return _printDebug;
}

bool SliceParameters::getPrintStatistics() const {
  return _printStatistics;
}
