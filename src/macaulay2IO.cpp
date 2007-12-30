#include "stdinc.h"
#include "macaulay2IO.h"

#include "monosIO.h"
#include <sstream>

class Macaulay2IdealWriter : public IdealWriter {
public:
  Macaulay2IdealWriter(FILE* file, const VarNames& names):
    _justStartedWritingIdeal(true),
    _file(file),
    _names(names) {
    fputs("R = ZZ[{", _file);
    ASSERT(!names.empty());

    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      fputs(pre, _file);
      fputs(names.getName(i).c_str(), _file);
      pre = ", ";
    }
    fputs("}];\n", _file);
    fputs("I = monomialIdeal(", _file);
  }

  virtual ~Macaulay2IdealWriter() {
    fputs("\n);\n", _file);
  }

  virtual void consume(const vector<const char*>& term) {
    writeSeparator();
    writeTerm(term, _file);
  }

  virtual void consume(const vector<mpz_class>& term) {
    writeSeparator();
    writeTerm(term, _names, _file);
  }

private:
  void writeSeparator() {
    if (_justStartedWritingIdeal) {
      _justStartedWritingIdeal = false;
      fputs("\n", _file);
    }
    else
      fputs(",\n", _file);
  }

  bool _justStartedWritingIdeal;
  FILE* _file;
  VarNames _names;
};

IdealWriter* Macaulay2IOHandler::
createWriter(FILE* file, const VarNames& names) const {
  return new Macaulay2IdealWriter(file, names);
}

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
      fputs("ERROR: a variable appears twice in irreducible ideal.\n", stderr);
      exit(1);
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
      fprintf(stderr,
	      "ERROR (on line %u): Variable \"%s\" is declared twice.\n",
	      lexer.getLineNumber(), varName.c_str());
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
