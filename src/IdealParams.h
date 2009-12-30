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
#ifndef IDEAL_PARAMS_GUARD
#define IDEAL_PARAMS_GUARD

namespace Params { 
  class CliParams;

  class IdealParams {
  public:
	IdealParams();

	void idealIsMinimal(bool value) {_idealIsMinimal = value;}
	void printActions(bool value) {_printActions = value;}
	void setInputFormat(const string& value) {_inputFormat = value;}
	void setOutputFormat(const string& value) {_outputFormat = value;}

	bool getIdealIsMinimal() const {return _idealIsMinimal;}
	bool getPrintActions() const {return _printActions;}
	const string& getInputFormat() const {return _inputFormat;}
	const string& getOutputFormat() const {return _outputFormat;}

  private:
	bool _idealIsMinimal;
	bool _printActions;
	string _inputFormat;
	string _outputFormat;
  };

  void addSliceParams(CliParams& params);
  void extractCliValues(IdealParams& ideal, const CliParams& cli);  
}
using Params::IdealParams;

#endif
