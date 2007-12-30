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
  _writer(IOHandler::getIOHandler("monos")->createWriter(out, names)),
  _translator(translator) {
  _translator->makeStrings();
}

DecomWriter::~DecomWriter() {
  delete _writer;
}

void DecomWriter::consume(const Term& term) {
  for (size_t var = 0; var < _varCount; ++var)
    _tmp[var] = _translator->getExponentString(var, term[var] + 1);
  _writer->consume(_tmp);
}
