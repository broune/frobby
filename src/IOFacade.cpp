#include "stdinc.h"
#include "IOFacade.h"

#include "io.h"
#include "fourti2.h"
#include "BigIdeal.h"
#include "fplllIO.h"

#include <iterator>

IOFacade::IOFacade(bool printActions):
  Facade(printActions) {
}

bool IOFacade::isValidMonomialIdealFormat(const char* format) {
  beginAction("Validating monomial ideal format name.");

  IOHandler* handler = IOHandler::createIOHandler(format);
  bool valid = (handler != 0);
  delete handler;

  endAction();

  return valid;
}

void IOFacade::readIdeal(FILE* in, BigIdeal& ideal, const char* format) {
  beginAction("Reading monomial ideal.");

  IOHandler* handler = IOHandler::createIOHandler(format);
  ASSERT(handler != 0);

  handler->readIdeal(in, ideal);
  delete handler;

  endAction();
}

void IOFacade::writeIdeal(FILE* out, BigIdeal& ideal, const char* format) {
  beginAction("Writing monomial ideal.");

  IOHandler* handler = IOHandler::createIOHandler(format);
  ASSERT(handler != 0);

  handler->writeIdeal(out, ideal);
  delete handler;

  endAction();
}

void IOFacade::
readFrobeniusInstance(FILE* in, vector<mpz_class>& instance) {
  beginAction("Reading Frobenius instance.");

  ::readFrobeniusInstance(in, instance);

  endAction();
}

void IOFacade::readFrobeniusInstanceWithGrobnerBasis
(FILE* in, BigIdeal& ideal, vector<mpz_class>& instance) {
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

bool IOFacade::isValidLatticeFormat(const char* format) {
  beginAction("Validating lattice format name.");

  bool valid =
    strcmp(format, "4ti2") == 0 ||
    strcmp(format, "fplll") == 0;

  endAction();

  return valid;
}

void IOFacade::
readLattice(FILE* in, BigIdeal& ideal, const char* format) {
  beginAction("Reading lattice basis.");

  if (strcmp(format, "4ti2") == 0)
    fourti2::readLatticeBasis(in, ideal);
  else if (strcmp(format, "fplll") == 0)
    fplll::readLatticeBasis(in, ideal);
  else
    ASSERT(false);

  endAction();
}

void IOFacade::
writeLattice(FILE* out, const BigIdeal& ideal, const char* format) {
  beginAction("Writing lattice basis.");

  if (strcmp(format, "4ti2") == 0)
    fourti2::writeLatticeBasis(out, ideal);
  else if (strcmp(format, "fplll") == 0)
    fplll::writeLatticeBasis(out, ideal);
  else
    ASSERT(false);

  endAction();
}
