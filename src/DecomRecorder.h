#ifndef DECOM_RECORDER_GUARD
#define DECOM_RECORDER_GUARD

#include "DecomConsumer.h"
#include "Term.h"

class Ideal;
class Term;

class DecomRecorder : public DecomConsumer {
public:
  // DecomRecorder does not take over ownership of recordInto.
  DecomRecorder(Ideal* recordInto);
  virtual ~DecomRecorder();

  virtual void consume(const Term& term);

private:
  Ideal* _recordInto;
  Term _tmp;
};

#endif
