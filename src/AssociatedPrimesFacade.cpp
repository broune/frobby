#include "stdinc.h"
#include "AssociatedPrimesFacade.h"

#include "IrreducibleDecomFacade.h"
#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "uwe.h"

#include <sstream>

AssociatedPrimesFacade::AssociatedPrimesFacade(bool printActions):
  Facade(printActions) {
}

void AssociatedPrimesFacade::
computeAPUsingIrrDecom(BigIdeal& ideal,
			const IrreducibleDecomParameters& params,
			ostream& out) {
  // TODO: This really needs to be done better.

  stringstream tmp;
  IrreducibleDecomFacade facade(isPrintingActions(), params);
  facade.computeIrreducibleDecom(ideal, tmp);

  beginAction("Computing associated primes from irreducible decomposition.");

  // Recover the ideal written to tmp.
  IOFacade ioFacade(false);
  ioFacade.readIdeal(tmp, ideal);

  // Take radicals and then remove duplicates.
  for (size_t term = 0; term < ideal.size(); ++term)
    for (size_t var = 0; var < ideal.getNames().getVarCount(); ++var)
      if (ideal.getExponent(term, var) > 1)
	ideal.setExponent(term, var, 1);

  ideal.sortUnique();

  ioFacade.writeIdeal(out, ideal);

  endAction();
}

void AssociatedPrimesFacade::computeAPUsingUwe
(BigIdeal& ideal, ostream& out) {
  beginAction("Computing associated primes using the Uwe algorithm.\n");

  BigIdeal primes(ideal.getNames());
  ::computeAssociatedPrimes(ideal, primes);

  endAction();

  IOFacade ioFacade(isPrintingActions());
  ioFacade.writeIdeal(out, primes);
}
