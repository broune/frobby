#ifndef GENERATOR_IDEAL_ACTION_GUARD
#define GENERATOR_IDEAL_ACTION_GUARD

#include "Action.h"
#include "StringParameter.h"
#include "IntegerParameter.h"
#include "IOParameters.h"

class GenerateIdealAction : public Action {
 public:
  GenerateIdealAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  StringParameter _type;

  IntegerParameter _variableCount;
  IntegerParameter _generatorCount;
  IntegerParameter _exponentRange;

  IOParameters _io;
};

#endif
