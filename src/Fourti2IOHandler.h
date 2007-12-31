#ifndef FOURTI2_IO_HANDLER_GUARD
#define FOURTI2_IO_HANDLER_GUARD

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;

class Fourti2IOHandler : public IOHandler {
public:
  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);
  virtual void readIrreducibleDecomposition(Scanner& scanner, BigIdeal& decom);

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);

  // These are not supported. Use writeIdeal instead.
  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const;

  virtual const char* getFormatName() const;
};

#endif
