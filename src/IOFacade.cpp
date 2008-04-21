#include "stdinc.h"
#include "IOFacade.h"

#include "IOHandler.h"
#include "fourti2.h"
#include "BigIdeal.h"
#include "fplllIO.h"
#include "Scanner.h"

#include <iterator>

IOFacade::IOFacade(bool printActions):
  Facade(printActions) {
}

bool IOFacade::isValidMonomialIdealFormat(const string& format) {
  beginAction("Validating monomial ideal format name.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  bool valid = (handler != 0);

  endAction();

  return valid;
}

void IOFacade::readIdeal(Scanner& in, BigIdeal& ideal) {
  beginAction("Reading monomial ideal.");

  IOHandler* handler = in.getIOHandler();
  ASSERT(handler != 0);

  handler->readIdeal(in, ideal);

  endAction();
}

void IOFacade::readIdeals(Scanner& in, vector<BigIdeal*>& ideals) {
  beginAction("Reading monomial ideals.");

  IOHandler* handler = in.getIOHandler();
  ASSERT(handler != 0);

  while (handler->hasMoreInput(in)) {
    BigIdeal* ideal = new BigIdeal();
	handler->readIdeal(in, *ideal);
    ideals.push_back(ideal);
  }

  endAction();
}


void IOFacade::writeIdeal(FILE* out, BigIdeal& ideal, const string& format) {
  beginAction("Writing monomial ideal.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  ASSERT(handler != 0);

  handler->writeIdeal(out, ideal);

  endAction();
}

bool IOFacade::readAlexanderDualInstance
(Scanner& in, BigIdeal& ideal, vector<mpz_class>& term) {
  beginAction("Reading Alexander dual input.");

  IOHandler* handler = in.getIOHandler();
  ASSERT(handler != 0);

  handler->readIdeal(in, ideal);

  bool pointSpecified = false;
  if (handler->hasMoreInput(in)) {
	handler->readTerm(in, ideal.getNames(), term);
	pointSpecified = true;
  }

  endAction();  

  return pointSpecified;
}

void IOFacade::
readFrobeniusInstance(Scanner& in, vector<mpz_class>& instance) {
  beginAction("Reading Frobenius instance.");

  ::readFrobeniusInstance(in, instance);

  endAction();
}

void IOFacade::readFrobeniusInstanceWithGrobnerBasis
(Scanner& in, BigIdeal& ideal, vector<mpz_class>& instance) {
  beginAction("Reading frobenius instance with Grobner basis.");

  fourti2::readGrobnerBasis(in, ideal);
  ::readFrobeniusInstance(in, instance);

  if (instance.size() != ideal.getVarCount() + 1) {
    if (instance.empty())
      fputs("ERROR: There is no Frobenius instance at end of input.\n",
			stderr);
    else
      fputs("ERROR: The Frobenius instance at end of input does not have the\n"
			"amount of numbers that the first part of the input indicates.\n",
			stderr);
    exit(1);
  }

  endAction();
}

void IOFacade::
writeFrobeniusInstance(FILE* out, vector<mpz_class>& instance) {
  beginAction("Writing Frobenius instance.");

  for (size_t i = 0; i < instance.size(); ++i) {
    if (i > 0)
      fputc(' ', out);
    gmp_fprintf(out, "%Zd", instance[i].get_mpz_t());
  }
  fputc('\n', out);

  endAction();
}

bool IOFacade::isValidLatticeFormat(const string& format) {
  beginAction("Validating lattice format name.");

  bool valid = (format == "4ti2" || format == "fplll");

  endAction();

  return valid;
}

void IOFacade::
readLattice(Scanner& in, BigIdeal& ideal) {
  beginAction("Reading lattice basis.");

  if (in.getFormat() == "4ti2")
    fourti2::readLatticeBasis(in, ideal);
  else if (in.getFormat() == "fplll")
    fplll::readLatticeBasis(in, ideal);
  else {
    ASSERT(false);
  }

  endAction();
}

void IOFacade::
writeLattice(FILE* out, const BigIdeal& ideal, const string& format) {
  beginAction("Writing lattice basis.");

  if (format == "4ti2")
    fourti2::writeLatticeBasis(out, ideal);
  else if (format == "fplll")
    fplll::writeLatticeBasis(out, ideal);
  else {
    ASSERT(false);
  }

  endAction();
}
