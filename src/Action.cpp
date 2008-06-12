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

// The point of doing it this way is that only actions that are
// actually used get constructed and deallocation is automatic.
template<class ActionType>
void addIfPrefix(const string& str,
				 Action::ActionContainer& actions) {
  if (string(ActionType::staticGetName()).compare(0, str.size(), str) == 0) {
	static ActionType action;
	actions.push_back(&action);
  }
}

Action* Action::getAction(const string& prefix) {
  ActionContainer actions;
  getActions(prefix, actions);

  if (actions.empty()) {
    fprintf(stderr, "ERROR: No action has the prefix \"%s\".\n",
			prefix.c_str());
	exit(1);
  }

  if (actions.size() >= 2) {
    fprintf(stderr, "ERROR: Prefix \"%s\" is ambigous.\nPossibilities are:",
			prefix.c_str());
	for (size_t i = 0; i < actions.size(); ++i) {
	  fputc(' ', stderr);
	  fputs(actions[i]->getName(), stderr);
	}
	fputc('\n', stderr);
    exit(1);
  }

  ASSERT(actions.size() == 1);
  return actions.back();
}

void Action::getActions(const string& prefix, ActionContainer& actions) {
  addIfPrefix<HilbertAction>(prefix, actions);
  addIfPrefix<IrreducibleDecomAction>(prefix, actions);
  addIfPrefix<AlexanderDualAction>(prefix, actions);
  addIfPrefix<AssociatedPrimesAction>(prefix, actions);
  addIfPrefix<TransformAction>(prefix, actions);
  addIfPrefix<PolyTransformAction>(prefix, actions);

  addIfPrefix<IntersectionAction>(prefix, actions);
  addIfPrefix<GenerateIdealAction>(prefix, actions);
  addIfPrefix<FrobeniusAction>(prefix, actions);
  addIfPrefix<DynamicFrobeniusAction>(prefix, actions);
  addIfPrefix<GenerateFrobeniusAction>(prefix, actions);
  addIfPrefix<AnalyzeAction>(prefix, actions);
  addIfPrefix<LatticeFormatAction>(prefix, actions);

  addIfPrefix<HelpAction>(prefix, actions);
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
