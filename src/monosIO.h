#ifndef MONOS_IO_GUARD
#define MONOS_IO_GUARD

#include "io.h"

class Lexer;
class VarNames;
class BigIdeal;

class MonosIOHandler : public IOHandler {
public:
  MonosIOHandler();
  
  virtual void readIdeal(istream& in, BigIdeal& ideal);

  virtual void startWritingIdeal(FILE* out,
				 const VarNames& names);

  virtual void writeGeneratorOfIdeal(FILE* out,
				     const vector<mpz_class>& generator,
				     const VarNames& names);

  virtual void writeGeneratorOfIdeal(FILE* out,
				     const vector<const char*>& generator,
				     const VarNames& names);

  virtual void doneWritingIdeal(FILE* out);

  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom);

  virtual const char* getFormatName() const;

  virtual IOHandler* createNew() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);

  void readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer);

  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);

  bool _justStartedWritingIdeal;
};

#endif
