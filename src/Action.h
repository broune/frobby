#ifndef ACTION_GUARD
#define ACTION_GUARD

#include "BoolParameter.h"

class Parameter;

class Action {
 public:
  Action();
  virtual ~Action();

  virtual const char* getName() const = 0;
  virtual const char* getShortDescription() const = 0;
  virtual const char* getDescription() const = 0;
  virtual Action* createNew() const = 0;

  // processNonParameter() can be called at most once, and only if
  // acceptsNonParameter() returns true.
  virtual bool processNonParameter(const char* str);
  virtual bool acceptsNonParameter() const;

  virtual void obtainParameters(vector<Parameter*>& parameters) = 0;

  virtual void parseCommandLine(unsigned int tokenCount,
				const char** tokens);

  virtual void perform() = 0;

  // These methods are NOT thread safe.
  typedef vector<const Action*> ActionContainer;
  static const ActionContainer& getActions();
  static Action* createAction(const string& name);

 protected:
  BoolParameter _printActions;

 private:
  void processOption(const string& optionName,
		     const char** params,
		     unsigned int paramCount);

  static ActionContainer _actions;
  vector<Parameter*> _parameters;
};

#endif
