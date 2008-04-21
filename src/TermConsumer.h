#ifndef TERM_CONSUMER_GUARD
#define TERM_CONSUMER_GUARD

class Term;

class TermConsumer {
 public:
  virtual ~TermConsumer() {}

  virtual void consume(const Term& term) = 0;
};

#endif
