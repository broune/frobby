#ifndef DECOM_WRITER_GUARD
#define DECOM_WRITER_GUARD

#include "VarNames.h"
#include "TermConsumer.h"
#include "io.h"

#include <vector>

class IOHandler;
class TermTranslator;
class Term;

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

  IdealWriter* _writer;
  const TermTranslator* _translator;
};

#endif
