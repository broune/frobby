/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "CliParams.h"

#include "error.h"
#include "ParameterGroup.h"
#include "IntegerParameter.h"
#include "BoolParameter.h"
#include "StringParameter.h"

namespace Params {
  void CliParams::parseCommandLine(unsigned int tokenCount, const char** tokens) {
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

  void CliParams::processOption(const string& optionName,
								const char** params,
								unsigned int paramCount) {
	for (vector<Parameter*>::iterator it = _params.begin();
		 it != _params.end(); ++it) {
	  if ((*it)->process(params - 1, paramCount + 1))
		return;
	}

	reportError("Unknown option \"-" + optionName + "\".");
  }

  void CliParams::add(auto_ptr<Parameter> param) {
	// TODO: take over ownership
	add(*param.release());
  }

  void CliParams::add(Parameter& param) {
	_params.push_back(&param);
  }

  void CliParams::add(ParameterGroup& params) {
	for (ParameterGroup::iterator it = params.begin(); it != params.end(); ++it)
	  add(**it);
  }

  const Parameter& CliParams::getParam(const string& name) const {
	for (const_iterator it = _params.begin(); it != _params.end(); ++it)
	  if ((*it)->getName() == name)
		return **it;

	INTERNAL_ERROR("Unknown parameter " + name + ".");
	throw; // To shut compiler up.
  }

  bool getBool(const CliParams& params, const string& name) {
	const Parameter& param = params.getParam(name);
	ASSERT(dynamic_cast<const BoolParameter*>(&param) != 0);
	return static_cast<const BoolParameter&>(param);
  }

  const string& getString(const CliParams& params, const string& name) {
	const Parameter& param = params.getParam(name);
	ASSERT(dynamic_cast<const StringParameter*>(&param) != 0);
	return static_cast<const StringParameter&>(param);
  }

  unsigned int getInt(const CliParams& params, const string& name) {
	const Parameter& param = params.getParam(name);
	ASSERT(dynamic_cast<const IntegerParameter*>(&param) != 0);
	return static_cast<const IntegerParameter&>(param);
  }
}
