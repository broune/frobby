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
#ifndef SCARF_PARAMS_GUARD
#define SCARF_PARAMS_GUARD

#include "CommonParams.h"

#include <string>

class CliParams;

class ScarfParams : public CommonParams {
 public:
  ScarfParams(CliParams& cli);

  /** Returns true if deforming to a strongly generic ideal. Returns
   false otherwise. */
  const bool getDeformToStronglyGeneric() const {return _deformStrong;}
  void setDeformToStronglyGeneric(bool value) {_deformStrong = value;}

  /** Returns the name of the enumeration ordering. */
  const string& getEnumerationOrder() const {return _enumerationOrder;}
  void setEnumerationOrder(const string& name) {_enumerationOrder = name;}

  /** Returns the name of the deformation ordering. */
  const string& getDeformationOrder() const {return _deformationOrder;}
  void setDeformationOrder(const string& name) {_deformationOrder = name;}

 private:
  bool _deformStrong;
  string _enumerationOrder;
  string _deformationOrder;
};

void addScarfParams(CliParams& params);
void extractCliValues(ScarfParams& scarf, const CliParams& cli);

#endif
