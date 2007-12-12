#include "stdinc.h"
#include "monosIO.h"

#include "BigIdeal.h"
#include "Lexer.h"

MonosIOHandler::MonosIOHandler():
  _justStartedWritingIdeal(false) {
}
  
void MonosIOHandler::readIdeal(istream& in, BigIdeal& ideal) {
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

#include <cstdio>
#include <sstream>

void MonosIOHandler::startWritingIdeal(FILE* out,
				       const VarNames& names) {
  fputs("vars ", out);
  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
    fputs(pre, out);
    fputs(names.getName(i).c_str(), out);
    pre = ", ";
  }
  fputs(";\n[", out);
  
  _justStartedWritingIdeal = true;
}

void MonosIOHandler::writeGeneratorOfIdeal(FILE* out,
					   const vector<mpz_class>& generator,
					   const VarNames& names) {
  if (_justStartedWritingIdeal) {
    _justStartedWritingIdeal = false;
    fputs("\n ", out);
  } else
    fputs(",\n ", out);

  bool someVar = false;
  size_t varCount = names.getVarCount();
  for (size_t j = 0; j < varCount; ++j) {
    if ((generator[j]) == 0)
      continue;
    if (someVar)
      putchar('*');
    else
      someVar = true;

    fputs(names.getName(j).c_str(), out);
    if ((generator[j]) != 1) {
      putchar('^');
      stringstream o;
      o << generator[j];
      fputs(o.str().c_str(), out);
    }
  }
  if (!someVar)
    putchar('1');
}

void MonosIOHandler::writeGeneratorOfIdeal
(FILE* out,
 const vector<const char*>& generator,
 const VarNames& names) {
  if (_justStartedWritingIdeal) {
    _justStartedWritingIdeal = false;
    putchar('\n');
  }
  else
    fputs(",\n", out);

  char separator = ' ';
  size_t varCount = names.getVarCount();
  for (size_t j = 0; j < varCount; ++j) {
    const char* exp = generator[j];
    if (exp == 0)
      continue;
  
    putchar(separator);
    fputs(exp, out);
    separator = '*';
  }

  if (separator == ' ')
    fputs(" 1", out);
}

void MonosIOHandler::doneWritingIdeal(FILE* out) {
  fputs("\n];\n", out);
}

void MonosIOHandler::readIrreducibleDecomposition(istream& in,
						  BigIdeal& decom) {
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
      cerr << "ERROR: a variable appears twice in irreducible ideal." << endl;
      exit(0);
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
      cerr << "ERROR (on line " << lexer.getLineNumber() << "): "
	   << "Variable \"" << varName << "\" is declared twice." << endl;
      exit(1);
    }

    names.addVar(varName);
  } while (lexer.match(','));

  lexer.expect(';');

  ideal.clearAndSetNames(names);
}
