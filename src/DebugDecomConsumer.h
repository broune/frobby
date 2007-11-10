#ifndef DEBUG_DECOM_CONSUMER
#define DEBUG_DECOM_CONSUMER

#include "DecomConsumer.h"
class Term;

class DebugDecomConsumer : public DecomConsumer {
public:
  DebugDecomConsumer(DecomConsumer* consumer);
  virtual ~DebugDecomConsumer();
  
  virtual void consume(const Term& term);
  
private:
  DecomConsumer* _consumer;
};

#endif
