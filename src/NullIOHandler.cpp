#include "stdinc.h"
#include "NullIOHandler.h"

#include "BigIdeal.h"

class NullIdealWriter : public IdealWriter {
public:
  NullIdealWriter() {
  }

  virtual ~NullIdealWriter() {
  }

  virtual void consume(const vector<mpz_class>& term) {
  }

  virtual void consume(const Term& term) {
  }
};

IdealWriter* NullIOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new NullIdealWriter();
}

IdealWriter* NullIOHandler::
createWriter(FILE* file, const TermTranslator* translator) const {
  return new NullIdealWriter();
}

void NullIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  VarNames names;
  ideal.clearAndSetNames(names);
}

void NullIOHandler::readIrreducibleDecomposition(Scanner& scanner,
												 BigIdeal& decom) {
}

bool NullIOHandler::hasMoreInput(Scanner& scanner) const {
  return false;
}

const char* NullIOHandler::getFormatName() const {
  return "null";
}
