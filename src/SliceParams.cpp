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
#include "stdinc.h"
#include "SliceParams.h"

#include "SplitStrategy.h"
#include "error.h"
#include "CliParams.h"

namespace Params { 
  SliceParams::SliceParams():
	_useIndependence(true) {
  }

  SliceParams& SliceParams::setSplit(const string& name) {
	_split = SplitStrategy::createStrategy(name.c_str());
	if (_split.get() == 0) {
	  INTERNAL_ERROR("Unknown Slice split " + name + ".");
	}
	return *this;
  }

  SliceParams& SliceParams::useIndependenceSplits(bool value) {
	_useIndependence = value;
	return *this;
  }

  namespace {
	static const char* SplitParamName = "split";
	static const char* UseIndependenceName = "independence";
  }

  void addSliceParams(CliParams& params) {
  }

  void extractCliValues(SliceParams& slice, const CliParams& cli) {
	extractCliValues(static_cast<SliceLikeParams&>(slice), cli);
	slice.setSplit(getString(cli, SplitParamName));
	slice.useIndependenceSplits(getBool(cli, UseIndependenceName));
  }
}
