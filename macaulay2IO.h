#ifndef MACAULAY2_IO_GUARD
#define MACAULAY2_IO_GUARD

#include "Lexer.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "io.h"

class Macaulay2IOHandler : public IOHandler {
public:
  Macaulay2IOHandler():
    _justStartedWritingIdeal(false) {
  }

  virtual ~Macaulay2IOHandler() {
  }

  virtual void readIdeal(istream& in,
			 BigIdeal& ideal) {
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

  virtual void startWritingIdeal(ostream& out,
				 const VarNames& names) {
    out << "R = ZZ[{";
    ASSERT(!names.empty());

    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      out << pre << names.getName(i);
      pre = ", ";
    }
    out << "}];\n";

    out << "I = monomialIdeal(";

    _justStartedWritingIdeal = true;
  }

  virtual void writeGeneratorOfIdeal(ostream& out,
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

  virtual void doneWritingIdeal(ostream& out) {
    out << "\n);\n";
  }

  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom) {
    Lexer lexer(in);
    readVarsAndClearIdeal(decom, lexer);
    readIrreducibleIdealList(decom, lexer);
    lexer.expectEOF();
  }

  virtual const char* getFormatName() const {
    return "m2";
  }

  virtual IOHandler* createNew() const {
    return new Macaulay2IOHandler();
  }

private:
  bool _justStartedWritingIdeal;

  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
    ideal.newLastTerm();

    lexer.expect("monomialIdeal");
    lexer.expect('(');

    int var;
    mpz_class power;

    do {
      readVarPower(var, power, ideal.getNames(), lexer);
      ASSERT(power > 0);
      if (ideal.getLastTermExponentRef(var) != 0) {
	cout << "ERROR: a variable appears twice in irreducible ideal." << endl;
	exit(0);
      }
      ideal.getLastTermExponentRef(var) = power;
    } while (lexer.match(','));

    lexer.expect(')');
  }

  void readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer) {
    lexer.expect('{');
    if (lexer.match('}'))
      return;

    do {
      readIrreducibleIdeal(ideals, lexer);
    } while (lexer.match(','));

    lexer.expect('}');
    lexer.match(';');
  }

  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
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
      names.addVar(varName);
    } while (lexer.match(','));

    if (readBrace)
      lexer.expect('}');
    lexer.expect(']');
    lexer.match(';');

    ideal.clearAndSetNames(names);
  }
};

namespace macaulay2 {
  void computeDecomposition(const BigIdeal& input, BigIdeal& output);
  void computeDecomposition(const BigIdeal& input, const string& outputFile);
}

#endif
