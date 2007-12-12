#include "stdinc.h"
#include "DecomWriter.h"

#include "io.h"
#include "TermTranslator.h"
#include "Term.h"

DecomWriter::DecomWriter(const VarNames& names,
			 const TermTranslator* translator,
			 FILE* out):
  _tmp(names.getVarCount()),
  _varCount(names.getVarCount()),
  _names(names),
  _translator(translator),
  _out(out) {
  _ioHandler = IOHandler::createIOHandler("monos");
  _ioHandler->startWritingIdeal(out, names);
}

DecomWriter::~DecomWriter() {
  _ioHandler->doneWritingIdeal(_out);
  fflush(_out);
  
  delete _ioHandler;
}

void DecomWriter::consume(const Term& term) {
  for (size_t var = 0; var < _varCount; ++var)
    _tmp[var] = _translator->getExponentString(var, term[var] + 1);
  _ioHandler->writeGeneratorOfIdeal(_out, _tmp, _names);
}
