#include "stdinc.h"
#include "macaulay2IO.h"

#include "monosIO.h"
#include <sstream>

void Macaulay2IOHandler::readIdeal(FILE* in, BigIdeal& ideal) {
  Lexer lexer(in);
  readVarsAndClearIdeal(ideal, lexer);

  lexer.expect('I');
  lexer.expect('=');
  lexer.expect("monomialIdeal");
  lexer.expect('(');

  do
    readTerm(ideal, lexer);
  while (lexer.match(','));

  lexer.expect(')');
  lexer.match(';');
  lexer.expectEOF();
}

void Macaulay2IOHandler::startWritingIdeal(FILE* out,
					   const VarNames& names) {
  fputs("R = ZZ[{", out);
  ASSERT(!names.empty());

  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
    fputs(pre, out);
    fputs(names.getName(i).c_str(), out);
    pre = ", ";
  }
  fputs("}];\n", stdout);
  fputs("I = monomialIdeal(", stdout);

  _justStartedWritingIdeal = true;
}

void Macaulay2IOHandler::doneWritingIdeal(FILE* out) {
  fputs("\n);\n", out);
}

void Macaulay2IOHandler::readIrreducibleDecomposition(FILE* in,
						      BigIdeal& decom) {
  Lexer lexer(in);
  readVarsAndClearIdeal(decom, lexer);
  readIrreducibleIdealList(decom, lexer);
  lexer.expectEOF();
}

const char* Macaulay2IOHandler::getFormatName() const {
  return "m2";
}

IOHandler* Macaulay2IOHandler::createNew() const {
  return new Macaulay2IOHandler();
}

void Macaulay2IOHandler::readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
  ideal.newLastTerm();

  lexer.expect("monomialIdeal");
  lexer.expect('(');

  int var;
  mpz_class power;

  do {
    readVarPower(var, power, ideal.getNames(), lexer);
    ASSERT(power > 0);
    if (ideal.getLastTermExponentRef(var) != 0) {
      cerr << "ERROR: a variable appears twice in irreducible ideal." << endl;
      exit(0);
    }
    ideal.getLastTermExponentRef(var) = power;
  } while (lexer.match(','));

  lexer.expect(')');
}

void Macaulay2IOHandler::readIrreducibleIdealList(BigIdeal& ideals,
						  Lexer& lexer) {
  lexer.expect('{');
  if (lexer.match('}'))
    return;

  do {
    readIrreducibleIdeal(ideals, lexer);
  } while (lexer.match(','));

  lexer.expect('}');
  lexer.match(';');
}

void Macaulay2IOHandler::readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
  lexer.expect('R');
  lexer.expect('=');
  lexer.expect("ZZ");
  lexer.expect('[');

  // The enclosing braces are optional, but if the start brace is
  // there, then the end brace should be there too.
  bool readBrace = lexer.match('{'); 

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
  } while (lexer.match(','));

  if (readBrace)
    lexer.expect('}');
  lexer.expect(']');
  lexer.match(';');

  ideal.clearAndSetNames(names);
}
