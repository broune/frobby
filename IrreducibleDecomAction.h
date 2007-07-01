#ifndef IRREDUCIBLE_DECOM_ACTION
#define IRREDUCIBLE_DECOM_ACTION

#include "Action.h"
#include "IrreducibleDecomParameters.h"

class IrreducibleDecomAction : public Action {
 public:
  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IrreducibleDecomParameters _decomParameters;
};

#endif
