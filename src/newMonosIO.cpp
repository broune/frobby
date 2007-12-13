#include "stdinc.h"
#include "newMonosIO.h"

#include <sstream>

void NewMonosIOHandler::readIdeal(istream& in, BigIdeal& ideal) {
  Lexer lexer(in);
  lexer.expect('(');
  lexer.expect("monomial-ideal-with-order");
  readVarsAndClearIdeal(ideal, lexer);

  do {
    readTerm(ideal, lexer);
  } while (!lexer.match(')'));
}

void NewMonosIOHandler::startWritingIdeal(FILE* out,
				   const VarNames& names) {
  fputs("(monomial-ideal-with-order\n (lex-order", out);
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
    putc(' ', out);
    fputs(names.getName(i).c_str(), out);
  }
  fputs(")\n", out);
}

void NewMonosIOHandler::writeGeneratorOfIdeal(FILE* out,
				       const vector<mpz_class>& generator,
				       const VarNames& names) {
  writeTerm(out, generator, names);
  putc('\n', out);
}

void NewMonosIOHandler::writeGeneratorOfIdeal(FILE* out,
				       const vector<const char*>& generator,
				       const VarNames& names) {
  writeTerm(out, generator, names);
  putc('\n', out);
}

void NewMonosIOHandler::doneWritingIdeal(FILE* out) {
  fputs(")\n", out);
}

void NewMonosIOHandler::readIrreducibleDecomposition(istream& in,
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
	cerr << "ERROR: a variable appears twice in irreducible ideal." << endl;
	exit(0);
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
      cerr << "ERROR (on line " << lexer.getLineNumber() << "): "
	   << "Variable \"" << varName << "\" is declared twice." << endl;
      exit(1);
    }

    names.addVar(varName);
  } while (!lexer.match(')'));

  ideal.clearAndSetNames(names);
}
