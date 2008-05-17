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
#include "AssociatedPrimesFacade.h"

#include "IrreducibleDecomFacade.h"
#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "Ideal.h"
#include "DecomRecorder.h"
#include "TermTranslator.h"

#include <sstream>

AssociatedPrimesFacade::AssociatedPrimesFacade(bool printActions):
  Facade(printActions) {
}

void AssociatedPrimesFacade::
computeAPUsingIrrDecom(BigIdeal& bigIdeal,
					   const IrreducibleDecomParameters& params,
					   FILE* out,
					   const string& format) {
  IrreducibleDecomFacade facade(isPrintingActions(), params);
  
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();

  Ideal decom(ideal.getVarCount());

  if (ideal.getGeneratorCount() > 0)
	translator.addArtinianPowers(ideal);

  facade.computeIrreducibleDecom(ideal, new DecomRecorder(&decom));

  beginAction("Computing associated primes from irreducible decomposition.");


  Ideal radical(bigIdeal.getVarCount());
  Term tmp(bigIdeal.getVarCount());

  Ideal::const_iterator stop = decom.end();
  for (Ideal::const_iterator it = decom.begin(); it != stop; ++it) {
    for (size_t var = 0; var < decom.getVarCount(); ++var) {
      if (translator.getExponent(var, (*it)[var]) == 0)
		tmp[var] = 0;
      else
		tmp[var] = 1;
    }
    radical.insert(tmp);
  }
  decom.clear();

  radical.removeDuplicates();

  bigIdeal.clear();
  bigIdeal.insert(radical);
  radical.clear();

  IOFacade ioFacade(isPrintingActions());
  ioFacade.writeIdeal(out, bigIdeal, format);

  endAction();
}
