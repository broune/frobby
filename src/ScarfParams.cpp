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
#include "BoolParameter.h"

ScarfParams::ScarfParams(CliParams& cli):
  _deformStrong(false),
  _enumerationOrder("revlex"),
  _deformationOrder("tdeg_revlex") {
  extractCliValues(*this, cli);
}

namespace {
  static const char* DeformToStrong = "deformStrong";
  static const char* EnumerationOrder = "enum";
  static const char* DeformationOrder = "deformationOrder";
}

void addScarfParams(CliParams& params) {
  ASSERT(!params.hasParam(DeformToStrong));
  params.add
	(auto_ptr<Parameter>
	 (new BoolParameter
	  (DeformToStrong,
	   "Deform to a strongly generic ideal if true. "
	   "Otherwise deform to a weakly generic ideal.",
	   false)));

  ASSERT(!params.hasParam(EnumerationOrder));
  params.add
	(auto_ptr<Parameter>
	 (new StringParameter
	  (EnumerationOrder,
	   "The enumeration order used for the deformation algorithm.",
	   "revlex")));

  ASSERT(!params.hasParam(DeformationOrder));
  params.add
	(auto_ptr<Parameter>
	 (new StringParameter
	  (DeformationOrder,
	   "The deformation order used for the deformation algorithm.",
	   "tdeg_revlex")));
}

void extractCliValues(ScarfParams& scarf, const CliParams& cli) {
  extractCliValues(static_cast<CommonParams&>(scarf), cli);
  scarf.setDeformToStronglyGeneric(getBool(cli, DeformToStrong));
  scarf.setEnumerationOrder(getString(cli, EnumerationOrder));
  scarf.setDeformationOrder(getString(cli, DeformationOrder));
}
