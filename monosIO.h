#ifndef MONOS_IO_GUARD
#define MONOS_IO_GUARD

#include "BigIdeal.h"
#include "Lexer.h"
#include "io.h"

#include <fstream>

class MonosIOHandler : public IOHandler {
public:
  MonosIOHandler():
    _justStartedWritingIdeal(false) {
  }
  
  virtual ~MonosIOHandler() {
  }

  virtual void readIdeal(istream& in, BigIdeal& ideal) {
    Lexer lexer(in);
    readVarsAndClearIdeal(ideal, lexer);

    lexer.expect('[');
    do {
      readTerm(ideal, lexer);
    } while (lexer.match(','));
    lexer.expect(']');
  }

  virtual void startWritingIdeal(ostream& out,
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
    out << "\n];\n";
  }

  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom) {
    Lexer lexer(in);
    readVarsAndClearIdeal(decom, lexer);
    readIrreducibleIdealList(decom, lexer);
  }

  virtual const char* getFormatName() const {
    return "monos";
  }

  virtual IOHandler* createNew() const {
    return new MonosIOHandler();
  }

private:
  bool _justStartedWritingIdeal;


  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer) {
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
	cout << "ERROR: a variable appears twice in irreducible ideal." << endl;
	exit(0);
      }
      ideal.getLastTermExponentRef(var) = power;
    } while (lexer.match(','));

    lexer.expect(']');
  }

  void readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer) {
    lexer.expect('[');
    if (lexer.match(']'))
      return;

    do {
      readIrreducibleIdeal(ideals, lexer);
    } while (lexer.match(',')); 

    lexer.expect(']');
    lexer.expect(';');
  }

  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer) {
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
};

namespace monos {
  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    BigIdeal& output,
			    bool printProgress);

  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    const string& outputFile,
			    bool printProgress);

  bool areIdealsEqual(const string& a, const string& b);
}

#endif
