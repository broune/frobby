#include "stdinc.h"
#include "Action.h"

#include "IrreducibleDecomAction.h"
#include "FormatAction.h"
#include "HelpAction.h"
#include "DynamicFrobeniusAction.h"
#include "GenerateIdealAction.h"
#include "GenerateFrobeniusAction.h"
#include "FrobeniusAction.h"
#include "AnalyzeAction.h"
#include "LatticeFormatAction.h"

Action::ActionContainer Action::_actions;

Action::Action():
  _printActions("time", "Display and time each subcomputation.", false) {
}

Action::~Action() {
}

const Action::ActionContainer& Action::getActions() {
  if (_actions.empty()) {
    _actions.push_back(new IrreducibleDecomAction());
    _actions.push_back(new FormatAction());
    _actions.push_back(new HelpAction());
    _actions.push_back(new AnalyzeAction());
    _actions.push_back(new GenerateIdealAction());
    _actions.push_back(new GenerateFrobeniusAction());
    _actions.push_back(new FrobeniusAction());
    _actions.push_back(new DynamicFrobeniusAction());
    _actions.push_back(new LatticeFormatAction());
  }

  return _actions;
}

Action* Action::createAction(const string& name) {
  getActions();
  for (ActionContainer::const_iterator it = _actions.begin();
       it != _actions.end(); ++it) {
    if (name == (*it)->getName())
      return (*it)->createNew();
  }

  return 0;
}

bool Action::acceptsNonParameter() const {
  return false;
}

bool Action::processNonParameter(const char* str) {
  ASSERT(false);

  return false;
}

void Action::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_printActions);
}

void Action::processOption(const string& optionName,
			   const char** params,
			   unsigned int paramCount) {
  params[-1] += 1;
  for (vector<Parameter*>::iterator it = _parameters.begin();
       it != _parameters.end(); ++it) {
    if ((*it)->process(params - 1, paramCount + 1))
      return;
  }

  cerr << "ERROR: Unknown option \"-" << optionName << "\"." << endl;
  exit(0);
}

void Action::parseCommandLine(unsigned int tokenCount, const char** tokens) {
  obtainParameters(_parameters);

  unsigned int i = 0;
  while (i < tokenCount) {
    ASSERT(tokens[i][0] != '\0');

    if (tokens[i][0] != '-') {
      cerr << "ERROR: Expected an option when reading \""
	   << tokens[i][0] << "\", but options start with a dash (-)."
	   << endl;
      exit(0);
    }

    unsigned int paramCount = 0;
    while (i + 1 + paramCount < tokenCount &&
	   tokens[i + 1 + paramCount][0] != '-')
      ++paramCount;

    string optionName(tokens[i] + 1);
    processOption(optionName, tokens + i + 1, paramCount);

    i += paramCount + 1;
  }
}
