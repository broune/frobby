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
#include "CommonParams.h"

#include "CliParams.h"
#include "IOHandler.h"

CommonParams::CommonParams():
  _idealIsMinimal(false),
  _printActions(false),
  _produceCanonicalOutput(false),
  _inputFormat(getFormatNameIndicatingToGuessTheInputFormat()), 
  _outputFormat(getFormatNameIndicatingToUseInputFormatAsOutputFormat()) {
}

namespace {
  static const char* IdealIsMinimalName = "minimal";
  static const char* PrintActionsName = "time";
  static const char* CanonicalName = "canon";
  static const char* InputFormatParamName = "iformat";
  static const char* OutputFormatParamName = "oformat";
}

void addCommonParams(CliParams& params) {
}

void extractCliValues(CommonParams& common, const CliParams& cli) {
  common.idealIsMinimal(getBool(cli, IdealIsMinimalName));
  common.printActions(getBool(cli, PrintActionsName));
  common.produceCanonicalOutput(getBool(cli, CanonicalName));
  // TODO: check both parameters exist.
  if (cli.hasParam(InputFormatParamName))
	common.setInputFormat(getString(cli, InputFormatParamName));
  if (cli.hasParam(OutputFormatParamName))
	common.setOutputFormat(getString(cli, OutputFormatParamName));
}
