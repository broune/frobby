#ifndef MACAULAY2_IO_GUARD
#define MACAULAY2_IO_GUARD

#include "BigIdeal.h"
#include "VarNames.h"
#include "io.h"

class Scanner;

class Macaulay2IOHandler : public IOHandler {
public:
  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);
  virtual void readIrreducibleDecomposition(Scanner& scanner, BigIdeal& decom);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const;

  virtual const char* getFormatName() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Scanner& scanner);
  void readIrreducibleIdealList(BigIdeal& ideals, Scanner& scanner);
  void readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner);
};

#endif
