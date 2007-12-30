#ifndef MACAULAY2_IO_GUARD
#define MACAULAY2_IO_GUARD

#include "Lexer.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "io.h"

class Macaulay2IOHandler : public IOHandler {
public:
  virtual void readIdeal(FILE*, BigIdeal& ideal);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;

  virtual void readIrreducibleDecomposition(FILE* in,
					    BigIdeal& decom);
  virtual const char* getFormatName() const;
  virtual IOHandler* createNew() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);
  void readIrreducibleIdealList(BigIdeal& ideals, Lexer& lexer);
  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);
};

#endif
