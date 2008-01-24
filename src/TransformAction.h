#ifndef TRANSFORM_ACTION_GUARD
#define TRANSFORM_ACTION_GUARD

#include "Action.h"
#include "IOParameters.h"
#include "BoolParameter.h"

class TransformAction : public Action {
 public:
  TransformAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IOParameters _io;
  BoolParameter _canonicalize;
  BoolParameter _sort;
  BoolParameter _unique;
  BoolParameter _minimize;
  BoolParameter _deform;
  BoolParameter _radical;
};

#endif
