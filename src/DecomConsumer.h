#ifndef DECOM_CONSUMER_GUARD
#define DECOM_CONSUMER_GUARD

class DecomConsumer {
 public:
  virtual ~DecomConsumer() {}
  
  virtual void consume(const Term& term) = 0;
};

#endif
