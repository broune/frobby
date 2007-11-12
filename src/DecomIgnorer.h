#ifndef DECOM_IGNORER_GUARD
#define DECOM_IGNORER_GUARD

#include "DecomConsumer.h"

class Term;

// Simply ignores everything it consumes.
class DecomIgnorer : public DecomConsumer {
 public:
  virtual void consume(const Term& term) {}
};

#endif
