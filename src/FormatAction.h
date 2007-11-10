#ifndef FORMAT_ACTION_GUARD
#define FORMAT_ACTION_GUARD

#include "Action.h"
#include "StringParameter.h"
#include "BoolParameter.h"

class FormatAction : public Action {
 public:
  FormatAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  StringParameter _inputFormat;
  StringParameter _outputFormat;
  BoolParameter _canonicalize;
  BoolParameter _sort;
  BoolParameter _unique;
  BoolParameter _minimize;
};

#endif
