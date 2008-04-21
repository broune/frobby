#include "stdinc.h"
#include "Action.h"

#include "IrreducibleDecomAction.h"
#include "TransformAction.h"
#include "HelpAction.h"
#include "DynamicFrobeniusAction.h"
#include "GenerateIdealAction.h"
#include "GenerateFrobeniusAction.h"
#include "FrobeniusAction.h"
#include "AnalyzeAction.h"
#include "LatticeFormatAction.h"
#include "IntersectAction.h"
#include "AssociatedPrimesAction.h"
#include "PrimaryDecomAction.h"
#include "AlexanderDualAction.h"

Action::ActionContainer Action::_actions;

Action::Action():
  _printActions("time", "Display and time each subcomputation.", false) {
}

Action::~Action() {
}

const Action::ActionContainer& Action::getActions() {
  if (_actions.empty()) {
    // This method uses static variables instead of new to avoid
    // spurious reports from memory leak detectors.

    static IrreducibleDecomAction irreducibleDecom;
    _actions.push_back(&irreducibleDecom);

	static AlexanderDualAction alexanderDual;
	_actions.push_back(&alexanderDual);

    static AssociatedPrimesAction associatedPrimes;
    _actions.push_back(&associatedPrimes);

    static IntersectAction intersect;
    _actions.push_back(&intersect);

    static GenerateIdealAction generateIdeal; 
    _actions.push_back(&generateIdeal);

    static TransformAction transform;
    _actions.push_back(&transform);

    static FrobeniusAction frobenius;
    _actions.push_back(&frobenius);

    static DynamicFrobeniusAction dynamicFrobenius;
    _actions.push_back(&dynamicFrobenius);

    static GenerateFrobeniusAction generateFrobenius;
    _actions.push_back(&generateFrobenius);

    static AnalyzeAction analyze;
    _actions.push_back(&analyze);

    static LatticeFormatAction latticeFormat;
    _actions.push_back(&latticeFormat);

    static HelpAction help;
    _actions.push_back(&help);

    //static PrimaryDecomAction primaryDecom;
    //_actions.push_back(&primaryDecom);
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

  fprintf(stderr, "ERROR: Unknown option \"-%s\".\n",
		  optionName.c_str());
  exit(1);
}

void Action::parseCommandLine(unsigned int tokenCount, const char** tokens) {
  if (acceptsNonParameter() && tokenCount > 0 && tokens[0][0] != '-') {
    if (!processNonParameter(tokens[0]))
      exit(1);
    --tokenCount;
    ++tokens;
  }

  obtainParameters(_parameters);

  unsigned int i = 0;
  while (i < tokenCount) {
    ASSERT(tokens[i][0] != '\0');

    if (tokens[i][0] != '-') {
      fprintf(stderr, "ERROR: Expected an option when reading "
	      "\"%s\", but options start with a dash (-).\n",
	      tokens[i]);
      exit(1);
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
