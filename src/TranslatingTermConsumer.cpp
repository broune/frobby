#include "stdinc.h"
#include "TranslatingTermConsumer.h"

#include "Term.h"
#include "TermTranslator.h"
#include "BigTermConsumer.h"

TranslatingTermConsumer::TranslatingTermConsumer
(BigTermConsumer* consumer, TermTranslator* translator):
  _consumer(consumer),
  _translator(translator) {
  ASSERT(consumer != 0);
  ASSERT(translator != 0);
}

TranslatingTermConsumer::~TranslatingTermConsumer() {
}

void TranslatingTermConsumer::consume(const Term& term) {
  ASSERT(term.getVarCount() == _translator->getNames().getVarCount());

  _consumer->consume(term, _translator);
}
