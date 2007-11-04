#ifndef DECOM_WRITER_GUARD
#define DECOM_WRITER_GUARD

#include "VarNames.h"
#include "TermTranslator.h"
#include "io.h"
#include "DecomConsumer.h"
#include "Term.h"

class DecomWriter : public DecomConsumer {
public:
  DecomWriter(const VarNames& names,
	      const TermTranslator* translator,
	      ostream& out);
  
  virtual ~DecomWriter();
  
  virtual void consume(const Term& term);
  
private:
  vector<const char*> _tmp;
  size_t _varCount;

  IOHandler* _ioHandler;
  VarNames _names;
  const TermTranslator* _translator;
  ostream& _out;
};

#endif
