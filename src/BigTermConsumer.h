#ifndef BIG_TERM_CONSUMER_GUARD
#define BIG_TERM_CONSUMER_GUARD

class Term;
class TermTranslator;

class BigTermConsumer {
 public:
  virtual ~BigTermConsumer();

  virtual void consume(const Term& term, TermTranslator* translator) = 0;
  virtual void consume(mpz_ptr* term) = 0;
};

#endif
