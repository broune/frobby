#include "stdinc.h"
#include "monosIO.h"

#include "BigIdeal.h"
#include "Lexer.h"

#include <fstream>

MonosIOHandler::MonosIOHandler():
  _justStartedWritingIdeal(false) {
}
  
void MonosIOHandler::readIdeal(istream& in, BigIdeal& ideal) {
  Lexer lexer(in);
  readVarsAndClearIdeal(ideal, lexer);
  
  lexer.expect('[');
  do {
    readTerm(ideal, lexer);
  } while (lexer.match(','));
  lexer.expect(']');
  lexer.expect(';');
}

void MonosIOHandler::startWritingIdeal(ostream& out,
				       const VarNames& names) {
  out << "vars ";
  const char* pre = "";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
    out << pre << names.getName(i);
    pre = ", ";
  }
  out << ";\n[";
  
  _justStartedWritingIdeal = true;
}

void MonosIOHandler::writeGeneratorOfIdeal(ostream& out,
					   const vector<mpz_class>& generator,
					   const VarNames& names) {
  if (_justStartedWritingIdeal)
    _justStartedWritingIdeal = false;
  else
    out << ',';
  out << "\n ";
  
  bool someVar = false;
  for (unsigned int j = 0; j < names.getVarCount(); ++j) {
    if ((generator[j]) == 0)
      continue;
    if (someVar)
      out << '*';
    else
      someVar = true;

    out << names.getName(j);
    if ((generator[j]) != 1)
      out << '^' << (generator[j]);
  }
  if (!someVar)
    out << 1;
}

void MonosIOHandler::writeGeneratorOfIdeal
(ostream& out,
 const vector<const char*>& generator,
 const VarNames& names) {
  if (_justStartedWritingIdeal) {
    _justStartedWritingIdeal = false;
    out << '\n';
  }
  else
    out << ",\n";

  char separator = ' ';
  for (unsigned int j = 0; j < names.getVarCount(); ++j) {
    const char* exp = generator[j];
    if (exp == 0)
      continue;

    out << separator << exp;
    separator = '*';
  }
  if (separator == ' ')
    out << " 1";
}

void MonosIOHandler::doneWritingIdeal(ostream& out) {
  out << "\n];\n";
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
    names.addVar(varName);
  } while (lexer.match(','));

  lexer.expect(';');

  ideal.clearAndSetNames(names);
}
