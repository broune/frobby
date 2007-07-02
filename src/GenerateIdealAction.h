#ifndef GENERATOR_IDEAL_ACTION_GUARD
#define GENERATOR_IDEAL_ACTION_GUARD

#include "Action.h"
#include "GenerateIdealParameters.h"

class GenerateIdealAction : public Action {
 public:
  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  GenerateIdealParameters _parameters;
};

#endif
