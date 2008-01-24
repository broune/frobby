#ifndef FROBENIUS_ACTION_GUARD
#define FROBENIUS_ACTION_GUARD

#include "Action.h"
#include "IrreducibleDecomParameters.h"
#include "BoolParameter.h"

class FrobeniusAction : public Action {
 public:
  FrobeniusAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IrreducibleDecomParameters _decomParameters;
  BoolParameter _displaySolution;
};

#endif
