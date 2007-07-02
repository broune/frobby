#ifndef LATTICE_FORMAT_ACTION_GUARD
#define LATTICE_FORMAT_ACTION_GUARD

#include "Action.h"
#include "StringParameter.h"
#include "BoolParameter.h"

class LatticeFormatAction : public Action {
 public:
  LatticeFormatAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const ;
  virtual const char* getDescription() const;
  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  StringParameter _inputFormat;
  StringParameter _outputFormat;
  BoolParameter _zero;
};

#endif
