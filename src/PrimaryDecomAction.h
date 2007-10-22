#ifndef PRIMARY_DECOM_ACTION_GUARD
#define PRIMARY_DECOM_ACTION_GUARD

#include "Action.h"

class PrimaryDecomAction : public Action {
 public:
  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual Action* createNew() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual void perform();
};

#endif
