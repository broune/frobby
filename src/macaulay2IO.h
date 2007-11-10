#ifndef MACAULAY2_IO_GUARD
#define MACAULAY2_IO_GUARD

#include "Lexer.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "io.h"

class Macaulay2IOHandler : public IOHandler {
public:
  Macaulay2IOHandler();
  virtual void readIdeal(istream& in,
			 BigIdeal& ideal);
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
  bool _justStartedWritingIdeal;

  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);
  void readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer);
  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);
};

#endif
