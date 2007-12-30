#ifndef NEW_MONOS_IO
#define NEW_MONOS_IO

#include "monosIO.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "Lexer.h"

class NewMonosIOHandler : public IOHandler {
public:
  virtual void readIdeal(FILE* in, BigIdeal& ideal);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const;

  virtual void readIrreducibleDecomposition(FILE* in,
					    BigIdeal& decom);
  virtual const char* getFormatName() const;

  virtual IOHandler* createNew() const;

private:
  void readIrreducibleIdeal(BigIdeal& ideal, Lexer& lexer);
  void readVarsAndClearIdeal(BigIdeal& ideal, Lexer& lexer);
};

#endif
