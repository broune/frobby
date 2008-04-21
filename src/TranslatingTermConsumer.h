#ifndef TRANSLATING_TERM_CONSUMER_GUARD
#define TRANSLATING_TERM_CONSUMER_GUARD

#include "TermConsumer.h"

class BigTermConsumer;
class TermTranslator;
class Term;

class TranslatingTermConsumer : public TermConsumer {
 public:
  TranslatingTermConsumer(BigTermConsumer* consumer,
						  TermTranslator* translator);
  virtual ~TranslatingTermConsumer();

  virtual void consume(const Term& term);

 private:
  BigTermConsumer* _consumer;
  TermTranslator* _translator;
};

#endif
