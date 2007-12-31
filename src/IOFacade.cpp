#include "stdinc.h"
#include "IOFacade.h"

#include "io.h"
#include "fourti2.h"
#include "BigIdeal.h"
#include "fplllIO.h"
#include "Scanner.h"

#include <iterator>

IOFacade::IOFacade(bool printActions):
  Facade(printActions) {
}

bool IOFacade::isValidMonomialIdealFormat(const char* format) {
  beginAction("Validating monomial ideal format name.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  bool valid = (handler != 0);

  endAction();

  return valid;
}

void IOFacade::readIdeal(FILE* in, BigIdeal& ideal, const char* format) {
  beginAction("Reading monomial ideal.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  ASSERT(handler != 0);

  Scanner scanner(in);
  handler->readIdeal(scanner, ideal);

  endAction();
}

void IOFacade::readIdeals(FILE* in, vector<BigIdeal*>& ideals,
						  const char* format) {
  beginAction("Reading monomial ideals.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  ASSERT(handler != 0);

  Scanner scanner(in);
  while (!scanner.matchEOF()) {
    BigIdeal* ideal = new BigIdeal();
	handler->readIdeal(scanner, *ideal);
    ideals.push_back(ideal);
  }

  endAction();
}


void IOFacade::writeIdeal(FILE* out, BigIdeal& ideal, const char* format) {
  beginAction("Writing monomial ideal.");

  IOHandler* handler = IOHandler::getIOHandler(format);
  ASSERT(handler != 0);

  handler->writeIdeal(out, ideal);

  endAction();
}

void IOFacade::
readFrobeniusInstance(FILE* in, vector<mpz_class>& instance) {
  beginAction("Reading Frobenius instance.");

  Scanner scanner(in);
  ::readFrobeniusInstance(scanner, instance);

  endAction();
}

void IOFacade::readFrobeniusInstanceWithGrobnerBasis
(FILE* in, BigIdeal& ideal, vector<mpz_class>& instance) {
  beginAction("Reading frobenius instance with Grobner basis.");

  Scanner scanner(in);
  fourti2::readGrobnerBasis(scanner, ideal);
  ::readFrobeniusInstance(scanner, instance);

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

  Scanner scanner(in);
  if (strcmp(format, "4ti2") == 0)
    fourti2::readLatticeBasis(scanner, ideal);
  else if (strcmp(format, "fplll") == 0)
    fplll::readLatticeBasis(scanner, ideal);
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
