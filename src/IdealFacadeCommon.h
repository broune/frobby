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
#ifndef IDEAL_FACADE_COMMON
#define IDEAL_FACADE_COMMON

#include "Facade.h"
#include "TermTranslator.h"
#include "Ideal.h"

class IdealFacadeParams;

namespace Facades {

  class IdealFacadeCommon : public Facade {
  protected:
	IdealFacadeCommon(const IdealFacadeParams& params);

	void minimizeIdeal();

  private:
	bool _idealIsKnownMinimal;
	const auto_ptr<TermTranslator> _translator;
	Ideal _ideal;
  };

}
using Facades::IdealFacadeCommon;

#endif
