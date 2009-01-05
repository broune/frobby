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

#include "error.h"
#include "NameFactory.h"

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
#include "PrimaryDecomAction.h"
#include "OptimizeAction.h"
#include "MaximalStandardAction.h"

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

namespace {
  typedef NameFactory<Action> ActionFactory;

  ActionFactory getActionFactory() {
	ActionFactory factory;

	nameFactoryRegister<HilbertAction>(factory);
	nameFactoryRegister<IrreducibleDecomAction>(factory);
	nameFactoryRegister<PrimaryDecomAction>(factory);
	nameFactoryRegister<AlexanderDualAction>(factory);
	nameFactoryRegister<AssociatedPrimesAction>(factory);
	nameFactoryRegister<MaximalStandardAction>(factory);	
	nameFactoryRegister<OptimizeAction>(factory);

	nameFactoryRegister<TransformAction>(factory);
	nameFactoryRegister<PolyTransformAction>(factory);

	nameFactoryRegister<IntersectionAction>(factory);
	nameFactoryRegister<GenerateIdealAction>(factory);
	nameFactoryRegister<FrobeniusAction>(factory);
	nameFactoryRegister<DynamicFrobeniusAction>(factory);
	nameFactoryRegister<GenerateFrobeniusAction>(factory);
	nameFactoryRegister<AnalyzeAction>(factory);
	nameFactoryRegister<LatticeFormatAction>(factory);

	nameFactoryRegister<HelpAction>(factory);
	nameFactoryRegister<TestAction>(factory);

	return factory;
  }
}

void Action::addNamesWithPrefix(const string& prefix,
								vector<string>& names) {
  getActionFactory().addNamesWithPrefix(prefix, names);
}

auto_ptr<Action> Action::createActionWithPrefix(const string& prefix) {
  vector<string> names;
  addNamesWithPrefix(prefix, names);

  if (names.empty())
	reportError("No action has the prefix \"" + prefix + "\".\n");

  if (names.size() >= 2) {
	string err = "Prefix \"" + prefix + "\" is ambigous.\nPossibilities are:";
	for (vector<string>::iterator name = names.begin();
		 name != names.end(); ++name) {
	  err += ' ';
	  err += *name;
	}
	err += '\n';
	reportError(err);
  }

  ASSERT(names.size() == 1);

  return getActionFactory().createWithPrefix(prefix);
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

void Action::processNonParameter(const char*) {
  ASSERT(false);
  reportInternalError("Action::processNonParameter called.");
}

void Action::obtainParameters(vector<Parameter*>& parameters) {
  parameters.push_back(&_printActions);
}

void Action::processOption(const string& optionName,
						   const char** params,
						   unsigned int paramCount) {
  for (vector<Parameter*>::iterator it = _parameters.begin();
       it != _parameters.end(); ++it) {
    if ((*it)->process(params - 1, paramCount + 1))
      return;
  }

  reportError("Unknown option \"-" + optionName + "\".");
}

void Action::parseCommandLine(unsigned int tokenCount, const char** tokens) {
  if (acceptsNonParameter() && tokenCount > 0 && tokens[0][0] != '-') {
    processNonParameter(tokens[0]);
    --tokenCount;
    ++tokens;
  }

  obtainParameters(_parameters);

  unsigned int i = 0;
  while (i < tokenCount) {
    ASSERT(tokens[i][0] != '\0');

    if (tokens[i][0] != '-')
	  reportError(string("Expected an option when reading \"") +
				  tokens[i] + "\", but options start with a dash (-).\n");

    unsigned int paramCount = 0;
    while (i + 1 + paramCount < tokenCount &&
	   tokens[i + 1 + paramCount][0] != '-')
      ++paramCount;

    string optionName(tokens[i] + 1);
    processOption(optionName, tokens + i + 1, paramCount);

    i += paramCount + 1;
  }
}
