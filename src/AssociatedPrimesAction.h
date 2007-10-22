#ifndef ASSOCIATED_PRIMES_ACTION_GUARD
#define ASSOCIATED_PRIMES_ACTION_GUARD

#include "Action.h"
#include "StringParameter.h"

class AssociatedPrimesAction : public Action {
 public:
  AssociatedPrimesAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  StringParameter _algorithm;
};

#endif
