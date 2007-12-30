#include "stdinc.h"
#include "DecomRecorder.h"

#include "Ideal.h"

DecomRecorder::DecomRecorder(Ideal* recordInto):
  _recordInto(recordInto),
  _tmp(recordInto->getVarCount()) {
  ASSERT(recordInto != 0);
}

DecomRecorder::~DecomRecorder() {
}

void DecomRecorder::consume(const Term& term) {
  ASSERT(term.getVarCount() == _tmp.getVarCount());
  _recordInto->insert(term);
}
