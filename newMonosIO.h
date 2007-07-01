#ifndef NEW_MONOS_IO
#define NEW_MONOS_IO

#include "monosIO.h"

class NewMonosIOHandler : public IOHandler {
public:
  NewMonosIOHandler() {
  }
  
  virtual ~NewMonosIOHandler() {
  }

  virtual void readIdeal(istream& in, BigIdeal& ideal) {
    Lexer lexer(in);
    lexer.expect('(');
    lexer.expect("monomial-ideal-with-order");
    readVarsAndClearIdeal(ideal, lexer);

    do {
      readTerm(ideal, lexer);
    } while (!lexer.match(')'));
  }

  virtual void startWritingIdeal(ostream& out,
				 const VarNames& names) {
    out << "(monomial-ideal-with-order\n(lex-order";
    for (unsigned int i = 0; i < names.getVarCount(); ++i)
      out << ' ' << names.getName(i);
    out << ")\n";
  }

  virtual void writeGeneratorOfIdeal(ostream& out,
				     const vector<mpz_class>& generator,
				     const VarNames& names) {
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
    out << '\n';
  }

  virtual void doneWritingIdeal(ostream& out) {
    out << ")\n";
  }

  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom) {
    Lexer lexer(in);
    lexer.expect('(');
    lexer.expect("lexed-list-with-order");
    readVarsAndClearIdeal(decom, lexer);

    while (!lexer.match(')'))
      readIrreducibleIdeal(decom, lexer);
  }

  virtual const char* getFormatName() const {
    return "newmonos";
  }

  virtual IOHandler* createNew() const {
    return new NewMonosIOHandler();
  }

private:

  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
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
	  cout << "ERROR: a variable appears twice in irreducible ideal." << endl;
	  exit(0);
	}
	ideal.getLastTermExponentRef(var) = power;
      }
    }
  }

  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
    lexer.expect('(');
    lexer.expect("lex-order");

    VarNames names;
    string varName;
    do {
      lexer.readIdentifier(varName);
      names.addVar(varName);
    } while (!lexer.match(')'));

    ideal.clearAndSetNames(names);
  }
};

namespace newMonos {
  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    BigIdeal& output,
			    bool useScarfComplex,
			    bool printProgress);

  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    const string& outputFile,
			    bool useScarfComplex,
			    bool printProgress);
}

#endif
