/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
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
#include "ScarfParams.h"

#include "CliParams.h"
#include "StringParameter.h"

ScarfParams::ScarfParams(CliParams& cli):
  _enumerationOrder("revlex") {
  extractCliValues(*this, cli);
}

namespace {
  static const char* EnumerationOrder = "enum";
}

void addScarfParams(CliParams& params) {
  ASSERT(!params.hasParam(EnumerationOrder));
  params.add
	(auto_ptr<Parameter>
	 (new StringParameter
	  (EnumerationOrder,
	   "The enumeration order used for the deformation algorithm.",
	   "revlex")));
}

void extractCliValues(ScarfParams& scarf, const CliParams& cli) {
  extractCliValues(static_cast<CommonParams&>(scarf), cli);
  scarf.setEnumerationOrder(getString(cli, EnumerationOrder));
}
