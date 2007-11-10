#ifndef HELP_ACTION_GUARD
#define HELP_ACTION_GUARD

#include "Action.h"

class HelpAction : public Action {
 public:
  HelpAction();

  virtual const char* getName() const;
  virtual const char* getShortDescription() const;
  virtual const char* getDescription() const;

  virtual void obtainParameters(vector<Parameter*>& parameters);

  virtual bool acceptsNonParameter() const;
  virtual bool processNonParameter(const char* str);

  virtual Action* createNew() const;

  virtual void perform();

 private:
  void displayTopic();

  Action* _topic;
};

#endif
