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
			ostream& out) {
  IrreducibleDecomFacade facade(isPrintingActions(), params);
  
  Ideal ideal(bigIdeal.getVarCount());
  TermTranslator translator(bigIdeal, ideal);
  bigIdeal.clear();
  translator.addArtinianPowers(ideal);

  Ideal decom(ideal.getVarCount());

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
  ioFacade.writeIdeal(out, bigIdeal);

  endAction();
}
