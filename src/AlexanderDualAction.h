#ifndef ALEXANDER_DUAL_ACTION
#define ALEXANDER_DUAL_ACTION

#include "Action.h"
#include "IrreducibleDecomParameters.h"
#include "IOParameters.h"

class AlexanderDualAction : public Action {
 public:
  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IrreducibleDecomParameters _decomParameters;
  IOParameters _io;
};

#endif
