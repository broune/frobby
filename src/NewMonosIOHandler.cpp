#include "stdinc.h"
#include "NewMonosIOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"

class NewMonosIdealWriter : public IdealWriter {
public:
  NewMonosIdealWriter(FILE* file, const VarNames& names):
    IdealWriter(file, names) {
    writeHeader();
  }

  NewMonosIdealWriter(FILE* file, const TermTranslator* translator):
    IdealWriter(file, translator) {
    writeHeader();
  }

  virtual ~NewMonosIdealWriter() {
    fputs(")\n", _file);
  }

  virtual void consume(const vector<const char*>& term) {
    writeTerm(term, _file);
    putc('\n', _file);
  }

  virtual void consume(const vector<mpz_class>& term) {
    writeTerm(term, _names, _file);
    putc('\n', _file);
  }

  virtual void consume(const Term& term) {
    writeTerm(term, _translator, _file);
    putc('\n', _file);
  }

private:
  void writeHeader() {
    fputs("(monomial-ideal-with-order\n (lex-order", _file);
    for (unsigned int i = 0; i < _names.getVarCount(); ++i) {
      putc(' ', _file);
      fputs(_names.getName(i).c_str(), _file);
    }
    fputs(")\n", _file);
  }
};

void NewMonosIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  scanner.expect('(');
  scanner.expect("monomial-ideal-with-order");
  readVarsAndClearIdeal(ideal, scanner);

  while (!scanner.match(')'))
    readTerm(ideal, scanner);
}

IdealWriter* NewMonosIOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new NewMonosIdealWriter(file, names);
}

IdealWriter* NewMonosIOHandler::
createWriter(FILE* file, const TermTranslator* translator) const {
  return new NewMonosIdealWriter(file, translator);
}

void NewMonosIOHandler::readIrreducibleDecomposition(Scanner& scanner,
													 BigIdeal& decom) {
  scanner.expect('(');
  scanner.expect("lexed-list-with-order");
  readVarsAndClearIdeal(decom, scanner);

  while (!scanner.match(')'))
    readIrreducibleIdeal(decom, scanner);
}

const char* NewMonosIOHandler::getFormatName() const {
  return "newmonos";
}

void NewMonosIOHandler::readIrreducibleIdeal(BigIdeal& ideal, Scanner& scanner) {
  ideal.newLastTerm();

  scanner.expect('(');
  scanner.expect("monomial-ideal");

  if (scanner.match('1'))
	return;

  while (!scanner.match(')'))
	readVarPower(ideal.getLastTermRef(), ideal.getNames(), scanner);
}

void NewMonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  scanner.expect('(');
  scanner.expect("lex-order");

  VarNames names;
  string varName;
  while (!scanner.match(')')) {
	const char* varName = scanner.readIdentifier();
    if (names.contains(varName)) {
	  scanner.printError();
      fprintf(stderr, "The variable %s is declared twice.\n", varName);
      exit(1);
    }
    names.addVar(varName);
  }

  ideal.clearAndSetNames(names);
}
