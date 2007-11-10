#ifndef NEW_MONOS_IO
#define NEW_MONOS_IO

#include "monosIO.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "Lexer.h"

class NewMonosIOHandler : public IOHandler {
public:
  virtual void readIdeal(istream& in, BigIdeal& ideal);
  virtual void startWritingIdeal(ostream& out,
				 const VarNames& names);
  virtual void writeGeneratorOfIdeal(ostream& out,
				     const vector<mpz_class>& generator,
				     const VarNames& names);
  virtual void doneWritingIdeal(ostream& out);
  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom);
  virtual const char* getFormatName() const;

  virtual IOHandler* createNew() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);
  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);
};

#endif
