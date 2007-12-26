#ifndef DECOM_WRITER_GUARD
#define DECOM_WRITER_GUARD

#include "VarNames.h"
#include "TermConsumer.h"

class IOHandler;
class TermTranslator;
class Term;

#include <vector>

class DecomWriter : public TermConsumer {
public:
  DecomWriter(const VarNames& names,
	      const TermTranslator* translator,
	      FILE* out);
  
  virtual ~DecomWriter();
  
  virtual void consume(const Term& term);
  
private:
  vector<const char*> _tmp;
  size_t _varCount;

  IOHandler* _ioHandler;
  VarNames _names;
  const TermTranslator* _translator;
  FILE* _out;
};

#endif
