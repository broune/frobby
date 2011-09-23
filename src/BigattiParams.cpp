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
#include "BigattiParams.h"

#include "CliParams.h"

BigattiParams::BigattiParams(const CliParams& cli):
  _pivot("median"),
  _widenPivot(true),
  _useGenericBaseCase(true) {
  extractCliValues(*this, cli);
}

namespace {
  static const char* PivotParamName = "split";
  static const char* GenericBaseCaseName = "genericBase";
  static const char* WidenPivotName = "widenPivot";
}

void addBigattParams(CliParams& params) {
}

void extractCliValues(BigattiParams& bigatti, const CliParams& cli) {
  extractCliValues(static_cast<SliceLikeParams&>(bigatti), cli);
  bigatti.setPivot(getString(cli, PivotParamName));
  bigatti.useGenericBaseCase(getBool(cli, GenericBaseCaseName));
  bigatti.widenPivot(getBool(cli, WidenPivotName));
}
