#ifndef NEW_MONOS_IO
#define NEW_MONOS_IO

#include "monosIO.h"
#include "BigIdeal.h"
#include "VarNames.h"

class Scanner;

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
