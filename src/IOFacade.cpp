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

void IOFacade::readIdeal(istream& in, BigIdeal& ideal, const char* format) {
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
readFrobeniusInstance(istream& in, vector<mpz_class>& instance) {
  beginAction("Reading Frobenius instance.");

  ::readFrobeniusInstance(in, instance);

  endAction();
}

void IOFacade::readFrobeniusInstanceWithGrobnerBasis
(istream& in, BigIdeal& ideal, vector<mpz_class>& instance) {
  beginAction("Reading frobenius instance with Grobner basis.");

  fourti2::readGrobnerBasis(in, ideal);
  ::readFrobeniusInstance(in, instance);

  if (instance.size() != ideal.getVarCount() + 1) {
    if (instance.empty())
      cerr << "ERROR: There is no Frobenius instance at end of input.";
    else
      cerr <<
	"ERROR: The Frobenius instance at end of input does not have\n"
	"the amount of numbers that the first part of the input indicates.";
    cerr << endl;
    exit(0);
  }

  endAction();
}

void IOFacade::
writeFrobeniusInstance(ostream& out, vector<mpz_class>& instance) {
  beginAction("Writing Frobenius instance.");

  copy(instance.begin(), instance.end(),
       ostream_iterator<mpz_class>(out, "\n"));

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
readLattice(istream& in, BigIdeal& ideal, const char* format) {
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
writeLattice(ostream& out, const BigIdeal& ideal, const char* format) {
  beginAction("Writing lattice basis.");

  if (strcmp(format, "4ti2") == 0)
    fourti2::writeLatticeBasis(out, ideal);
  else if (strcmp(format, "fplll") == 0)
    fplll::writeLatticeBasis(out, ideal);
  else
    ASSERT(false);

  endAction();
}
