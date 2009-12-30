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
#include "SplitStrategy.h"

class Action;

namespace Params {
  class SliceParams : public SliceLikeParams {
  public:
	SliceParams();

	// TOOO: throw exception if invalid
	SliceParams& setSplit(const string& name);
	SliceParams& useIndependenceSplits(bool value);

  private:
	auto_ptr<SplitStrategy> _split;
	bool _useIndependence;
  };

  void addIdealParams(CliParams& params);
  void extractCliValues(SliceParams& slice, const CliParams& cli);
}

using Params::SliceParams;

#endif
