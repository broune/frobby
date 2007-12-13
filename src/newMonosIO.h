#ifndef NEW_MONOS_IO
#define NEW_MONOS_IO

#include "monosIO.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "Lexer.h"

class NewMonosIOHandler : public IOHandler {
public:
  virtual void readIdeal(istream& in, BigIdeal& ideal);
  virtual void startWritingIdeal(FILE* out,
				 const VarNames& names);
  virtual void writeGeneratorOfIdeal(FILE* out,
				     const vector<mpz_class>& generator,
				     const VarNames& names);
  void writeGeneratorOfIdeal(FILE* out,
			     const vector<const char*>& generator,
			     const VarNames& names);

  virtual void doneWritingIdeal(FILE* out);
  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom);
  virtual const char* getFormatName() const;

  virtual IOHandler* createNew() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);
  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);
};

#endif
