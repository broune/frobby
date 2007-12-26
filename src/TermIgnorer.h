#ifndef TERM_IGNORER_GUARD
#define TERM_IGNORER_GUARD

#include "TermConsumer.h"

class Term;

// Simply ignores everything it consumes.
class TermIgnorer : public TermConsumer {
 public:
  virtual void consume(const Term& term) {}
};

#endif
