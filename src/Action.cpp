/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
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
#include "IntersectionAction.h"
#include "AssociatedPrimesAction.h"
#include "AlexanderDualAction.h"
#include "HilbertAction.h"
#include "PolyTransformAction.h"
#include "HelpAction.h"
#include "TestAction.h"

Action::Action(const char* name,
			   const char* shortDescription,
			   const char* description,
			   bool acceptsNonParameterParam):
  _name(name),
  _shortDescription(shortDescription),
  _description(description),
  _acceptsNonParameter(acceptsNonParameterParam),
  _printActions("time", "Display and time each subcomputation.", false) {
}

Action::~Action() {
}

// This is helper code that makes use of template to save a lot of the
// typing required to do this directly by hand. Also, this has the
// benefit that this is the single place that each new ActionType has
// to be registered.
namespace {
  // Helper function for ActionFactory.
  template<class ActionType>
  Action* factoryMethod() {
	return new ActionType();
  }

  // No virtual methods means that we can copy these things around by
  // value and this avoid having to deal with memory allocation.
  class ActionFactory {
  public:
	ActionFactory(const char* name, Action* (*factoryMethodParam)()):
	  _name(name),
	  _factoryMethod(factoryMethodParam) {
	}
	
	const string& getName() const {
	  return _name;
	}

	Action* createAction() const {
	  return _factoryMethod();
	}

	bool hasPrefix(const string& prefix) const {
	  return getName().compare(0, prefix.size(), prefix) == 0;
	}

  private:
	string _name;
	Action* (*_factoryMethod)();
  };

  // Helper function for addActionFactories.
  template<class ActionType>
  void addActionFactory(vector<ActionFactory>& factories) {
	const char* name = ActionType::staticGetName();
	Action* (*factoryMethodParam)() = factoryMethod<ActionType>;

	factories.push_back(ActionFactory(name, factoryMethodParam));
  }

  void addActionFactories(vector<ActionFactory>& factories) {
	addActionFactory<HilbertAction>(factories);
	addActionFactory<IrreducibleDecomAction>(factories);
	addActionFactory<AlexanderDualAction>(factories);
	addActionFactory<AssociatedPrimesAction>(factories);
	addActionFactory<TransformAction>(factories);
	addActionFactory<PolyTransformAction>(factories);

	addActionFactory<IntersectionAction>(factories);
	addActionFactory<GenerateIdealAction>(factories);
	addActionFactory<FrobeniusAction>(factories);
	addActionFactory<DynamicFrobeniusAction>(factories);
	addActionFactory<GenerateFrobeniusAction>(factories);
	addActionFactory<AnalyzeAction>(factories);
	addActionFactory<LatticeFormatAction>(factories);

	addActionFactory<HelpAction>(factories);
	addActionFactory<TestAction>(factories);
  }
}

void Action::addNamesWithPrefix(const string& prefix,
								vector<string>& names) {
  vector<ActionFactory> factories;
  addActionFactories(factories);

  for (vector<ActionFactory>::const_iterator factory = factories.begin();
	   factory != factories.end(); ++factory)
	if (factory->hasPrefix(prefix))
	  names.push_back(factory->getName());
}

auto_ptr<Action> Action::createActionWithPrefix(const string& prefix) {
  vector<string> names;
  addNamesWithPrefix(prefix, names);

  if (names.empty()) {
    fprintf(stderr, "ERROR: No action has the prefix \"%s\".\n",
			prefix.c_str());
	exit(1);
  }

  if (names.size() >= 2) {
    fprintf(stderr, "ERROR: Prefix \"%s\" is ambigous.\nPossibilities are:",
			prefix.c_str());
	for (vector<string>::iterator name = names.begin();
		 name != names.end(); ++name) {
	  fputc(' ', stderr);
	  fputs(name->c_str(), stderr);
	}
	fputc('\n', stderr);
    exit(1);
  }

  ASSERT(names.size() == 1);

  vector<ActionFactory> factories;
  addActionFactories(factories);

  for (vector<ActionFactory>::const_iterator factory = factories.begin();
	   factory != factories.end(); ++factory)
	if (factory->hasPrefix(prefix))
	  return auto_ptr<Action>(factory->createAction());

  ASSERT(false);
  return auto_ptr<Action>();
}

const char* Action::getName() const {
  return _name;
}

const char* Action::getShortDescription() const {
  return _shortDescription;
}

const char* Action::getDescription() const {
  return _description;
}

bool Action::acceptsNonParameter() const {
  return _acceptsNonParameter;
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
