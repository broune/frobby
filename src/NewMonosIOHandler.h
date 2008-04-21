#ifndef NEW_MONOS_HANDLER_IO
#define NEW_MONOS_HANDLER_IO

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;

class NewMonosIOHandler : public IOHandler {
public:
  virtual void readIdeal(Scanner& in, BigIdeal& ideal);
  virtual void readIrreducibleDecomposition(Scanner& in, BigIdeal& decom);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const;

  virtual const char* getFormatName() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Scanner& scanner);
  void readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner);
};

#endif
