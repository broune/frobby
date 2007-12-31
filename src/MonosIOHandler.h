#ifndef MONOS_IO_HANDLER_GUARD
#define MONOS_IO_HANDLER_GUARD

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;

class MonosIOHandler : public IOHandler {
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
