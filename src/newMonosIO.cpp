#include "stdinc.h"
#include "newMonosIO.h"

#include <sstream>

class NewMonosIdealWriter : public IdealWriter {
public:
  NewMonosIdealWriter(FILE* file, const VarNames& names):
    _file(file),
    _names(names) {
    ASSERT(!names.empty());
    fputs("(monomial-ideal-with-order\n (lex-order", _file);
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      putc(' ', _file);
      fputs(names.getName(i).c_str(), _file);
    }
    fputs(")\n", _file);
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

private:
  FILE* _file;
  VarNames _names;
};

void NewMonosIOHandler::readIdeal(FILE* in, BigIdeal& ideal) {
  Lexer lexer(in);
  lexer.expect('(');
  lexer.expect("monomial-ideal-with-order");
  readVarsAndClearIdeal(ideal, lexer);

  do {
    readTerm(ideal, lexer);
  } while (!lexer.match(')'));
}

IdealWriter* NewMonosIOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new NewMonosIdealWriter(file, names);
}

void NewMonosIOHandler::readIrreducibleDecomposition(FILE* in,
						     BigIdeal& decom) {
  Lexer lexer(in);
  lexer.expect('(');
  lexer.expect("lexed-list-with-order");
  readVarsAndClearIdeal(decom, lexer);

  while (!lexer.match(')'))
    readIrreducibleIdeal(decom, lexer);
}

const char* NewMonosIOHandler::getFormatName() const {
  return "newmonos";
}

IOHandler* NewMonosIOHandler::createNew() const {
  return new NewMonosIOHandler();
}

void NewMonosIOHandler::readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
  ideal.newLastTerm();

  lexer.expect('(');
  lexer.expect("monomial-ideal");

  int var;
  mpz_class power;

  if (!lexer.match('1')) {
    while (!lexer.match(')')) {
      readVarPower(var, power, ideal.getNames(), lexer);
      ASSERT(power > 0);
      if (ideal.getLastTermExponentRef(var) != 0) {
	fputs("ERROR: a variable appears twice in irreducible ideal.\n",
	      stderr);
	exit(1);
      }
      ideal.getLastTermExponentRef(var) = power;
    }
  }
}

void NewMonosIOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
  lexer.expect('(');
  lexer.expect("lex-order");

  VarNames names;
  string varName;
  do {
    lexer.readIdentifier(varName);
    if (names.contains(varName)) {
      fprintf(stderr,
	      "ERROR (on line %u): Variable \"%s\" is declared twice.\n",
	      lexer.getLineNumber(), varName.c_str());
      exit(1);
    }

    names.addVar(varName);
  } while (!lexer.match(')'));

  ideal.clearAndSetNames(names);
}
