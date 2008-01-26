#ifndef INTERSECT_ACTION_GUARD
#define INTERSECT_ACTION_GUARD

#include "Action.h"
#include "IOParameters.h"

class IntersectAction : public Action {
 public:
  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();

 private:
  IOParameters _io;
};

#endif
