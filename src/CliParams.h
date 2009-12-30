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
#ifndef CLI_PARAMS_GUARD
#define CLI_PARAMS_GUARD

#include "Parameter.h"
#include "ElementDeleter.h"
#include <vector>
#include <map>

class ParameterGroup;

namespace Params {
  class CliParams {
  public:
	void add(auto_ptr<Parameter> param);

	// TODO: remove this!
	void add(Parameter& param);
	void add(ParameterGroup& params);

	const Parameter& getParam(const string& name) const;

	// TODO: get RID of this.
	vector<Parameter*>& getParamsREMOVE() {return _params;}

	void parseCommandLine(unsigned int tokenCount, const char** tokens);
	void processOption(const string& optionName,
					   const char** params,
					   unsigned int paramCount);

  private:
	typedef vector<Parameter*>::iterator iterator;
	typedef vector<Parameter*>::const_iterator const_iterator;

	vector<Parameter*> _params;
	//ElementDeleter _paramsDeleter; TODO: reinstate
  };

  bool getBool(const CliParams& params, const string& name);
  const string& getString(const CliParams& params, const string& name);
  unsigned int getInt(const CliParams& params, const string& name);
}
using Params::CliParams;

#endif
