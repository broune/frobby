#include "stdinc.h"
#include "MonosIOHandler.h"

#include "BigIdeal.h"
#include "Scanner.h"
#include <cstdio>
#include <sstream>

class MonosIdealWriter : public IdealWriter {
public:
  MonosIdealWriter(FILE* file, const VarNames& names):
    IdealWriter(file, names),
    _justStartedWritingIdeal(true) {
    writeHeader();
  }

  MonosIdealWriter(FILE* file, const TermTranslator* translator):
    IdealWriter(file, translator),
    _justStartedWritingIdeal(true) {
    writeHeader();
  }

  virtual ~MonosIdealWriter() {
    fputs("\n];\n", _file);
  }

  virtual void consume(const vector<const char*>& term) {
    writeSeparator();
    writeTerm(term, _file);
  }

  virtual void consume(const vector<mpz_class>& term) {
    writeSeparator();
    writeTerm(term, _names, _file);
  }

  virtual void consume(const Term& term) {
    writeSeparator();
    writeTerm(term, _translator, _file);
  }

private:
  void writeHeader() {
    fputs("vars ", _file);
    const char* pre = "";
    for (unsigned int i = 0; i < _names.getVarCount(); ++i) {
      fputs(pre, _file);
      fputs(_names.getName(i).c_str(), _file);
      pre = ", ";
    }
    fputs(";\n[", _file);
  }

  void writeSeparator() {
    if (_justStartedWritingIdeal) {
      _justStartedWritingIdeal = false;
      fputs("\n", _file);
    }
    else
      fputs(",\n", _file);
  }

  bool _justStartedWritingIdeal;
};

IdealWriter* MonosIOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new MonosIdealWriter(file, names);
}

IdealWriter* MonosIOHandler::
createWriter(FILE* file, const TermTranslator* translator) const {
  return new MonosIdealWriter(file, translator);
}

void MonosIOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  readVarsAndClearIdeal(ideal, scanner);
  
  scanner.expect('[');
  if (!scanner.match(']')) {
    do {
      readTerm(ideal, scanner);
    } while (scanner.match(','));
    scanner.expect(']');
  }
  scanner.expect(';');
}

void MonosIOHandler::readIrreducibleDecomposition(Scanner& scanner,
												  BigIdeal& decom) {
  readVarsAndClearIdeal(decom, scanner);
  readIrreducibleIdealList(decom, scanner);
}

const char* MonosIOHandler::getFormatName() const {
  return "monos";
}

void MonosIOHandler::readIrreducibleIdeal(BigIdeal& ideal, Scanner& scanner) {
  ideal.newLastTerm();

  scanner.expect('[');
  if (scanner.match(']'))
    return;

  do
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), scanner);
  while (scanner.match(','));

  scanner.expect(']');
}

void MonosIOHandler::readIrreducibleIdealList(BigIdeal& ideals, Scanner& scanner) {
  scanner.expect('[');
  if (scanner.match(']'))
    return;
  
  do {
    readIrreducibleIdeal(ideals, scanner);
  } while (scanner.match(',')); 

  scanner.expect(']');
  scanner.expect(';');
}

void MonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner) {
  scanner.expect("vars");

  VarNames names;
  if (!scanner.match(';')) {
	do {
	  const char* varName = scanner.readIdentifier();
	  if (names.contains(varName)) {
		scanner.printError();
		fprintf(stderr, "The variable %s is declared twice.\n", varName);
		exit(1);
	  }
	  
	  names.addVar(varName);
	} while (scanner.match(','));

	scanner.expect(';');
  }

  ideal.clearAndSetNames(names);
}
