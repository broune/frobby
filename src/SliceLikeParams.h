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
#ifndef SLICE_LIKE_PARAMS
#define SLICE_LIKE_PARAMS

#include "IdealParams.h"

namespace Params {
  class CliParams;

  class SliceLikeParams : public IdealParams {
  public:
	SliceLikeParams();

	void printDebug(bool value) {_printDebug = value;}
	void printStatistics(bool value) {_printStatistics = value;}
	void produceCanonicalOutput(bool value) {_produceCanonicalOutput = value;}
	void useSimplification(bool value) {_useSimplification = value;}

	bool getPrintDebug() const {return _printDebug;}
	bool getPrintStatistics() const {return _printStatistics;}
	bool getProduceCanonicalOutput() const {return _produceCanonicalOutput;}
	bool getUseSimplification() const {return _useSimplification;}

  private:
	bool _printDebug;
	bool _printStatistics;
	bool _produceCanonicalOutput;
	bool _useSimplification;
  };

  void addSliceLikeParams(CliParams& params);
  void extractCliValues(SliceLikeParams& slice, const CliParams& cli);
}

#endif
