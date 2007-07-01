#include "stdinc.h"
#include "IOFacade.h"

#include "io.h"
#include "fourti2.h"
#include "BigIdeal.h"

#include <iterator>

IOFacade::IOFacade(bool printActions):
  Facade(printActions) {
}

bool IOFacade::isValidMonomialIdealFormat(const char* format) {
  beginAction("Validating format name.");

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

void IOFacade::writeIdeal(ostream& out, BigIdeal& ideal, const char* format) {
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

  // TODO: give Facade an error(msg) method.
  if (instance.size() != ideal.getNames().getVarCount() + 1) {
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
       ostream_iterator<mpz_class>(cout, "\n"));

  endAction();
}
