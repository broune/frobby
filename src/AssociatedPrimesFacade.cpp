#include "stdinc.h"
#include "AssociatedPrimesFacade.h"

#include "IrreducibleDecomFacade.h"
#include "IrreducibleDecomParameters.h"
#include "BigIdeal.h"
#include "IOFacade.h"
#include "TermList.h"
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
  
  Ideal* ideal;
  TermTranslator* translator;
  bigIdeal.buildAndClear(ideal, translator, false);

  TermList decom(ideal->getVarCount());

  facade.computeIrreducibleDecom(ideal, new DecomRecorder(&decom));

  beginAction("Computing associated primes from irreducible decomposition.");

  // Take radical.
  Ideal::iterator stop = decom.end();
  for (Ideal::iterator it = decom.begin(); it != stop; ++it) {
    for (size_t var = 0; var < decom.getVarCount(); ++var) {
      if (translator->getExponent(var, (*it)[var]) == 0)
	(*it)[var] = 0;
      else
	(*it)[var] = 1;
    }
  }
  delete translator;

  decom.removeDuplicates();

  bigIdeal.clear();
  bigIdeal.insert(decom);

  IOFacade ioFacade(isPrintingActions());
  ioFacade.writeIdeal(out, bigIdeal);

  endAction();
}
