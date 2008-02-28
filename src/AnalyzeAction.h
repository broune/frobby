#ifndef ANALYZE_ACTION_GUARD
#define ANALYZE_ACTION_GUARD

#include "Action.h"
#include "BoolParameter.h"
#include "IOParameters.h"

class AnalyzeAction : public Action {
 public:
  AnalyzeAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IOParameters _io;
  BoolParameter _printLcm;
  BoolParameter _printLabels;
  BoolParameter _printVarCount;
  BoolParameter _printGeneratorCount;
  BoolParameter _printMaximumExponent;
  BoolParameter _printMinimal;
};

#endif
