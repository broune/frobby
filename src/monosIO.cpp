#include "stdinc.h"
#include "monosIO.h"

#include "BigIdeal.h"
#include "Lexer.h"
#include <cstdio>
#include <sstream>

MonosIOHandler::MonosIOHandler() {
}

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

void MonosIOHandler::readIdeal(FILE* in, BigIdeal& ideal) {
  Lexer lexer(in);
  readVarsAndClearIdeal(ideal, lexer);
  
  lexer.expect('[');
  if (!lexer.match(']')) {
    do {
      readTerm(ideal, lexer);
    } while (lexer.match(','));
    lexer.expect(']');
  }
  lexer.expect(';');
}

void MonosIOHandler::readIrreducibleDecomposition(FILE* in, BigIdeal& decom) {
  Lexer lexer(in);
  readVarsAndClearIdeal(decom, lexer);
  readIrreducibleIdealList(decom, lexer);
}

const char* MonosIOHandler::getFormatName() const {
  return "monos";
}

IOHandler* MonosIOHandler::createNew() const {
  return new MonosIOHandler();
}

void MonosIOHandler::readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
  ideal.newLastTerm();

  lexer.expect('[');
  if (lexer.match(']'))
    return;

  int var;
  mpz_class power;

  do {
    readVarPower(var, power, ideal.getNames(), lexer);
    ASSERT(power > 0);
    if (ideal.getLastTermExponentRef(var) != 0) {
      fputs("ERROR: a variable appears twice in irreducible ideal.\n", stderr);
      exit(1);
    }
    ideal.getLastTermExponentRef(var) = power;
  } while (lexer.match(','));

  lexer.expect(']');
}

void MonosIOHandler::readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer) {
  lexer.expect('[');
  if (lexer.match(']'))
    return;
  
  do {
    readIrreducibleIdeal(ideals, lexer);
  } while (lexer.match(',')); 

  lexer.expect(']');
  lexer.expect(';');
}

void MonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
  lexer.expect("vars");

  VarNames names;
  string varName;
  do {
    lexer.readIdentifier(varName);
    if (names.contains(varName)) {
      fprintf(stdout,
	      "ERROR (on line %u): Variable \"%s\" is declared twice.\n",
	      lexer.getLineNumber(), varName.c_str());
      exit(1);
    }

    names.addVar(varName);
  } while (lexer.match(','));

  lexer.expect(';');

  ideal.clearAndSetNames(names);
}
