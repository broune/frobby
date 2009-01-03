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
#include "HelpAction.h"

#include "Parameter.h"
#include "IOHandler.h"
#include "error.h"
#include "DataType.h"

HelpAction::HelpAction():
  Action
(staticGetName(),
 "Display this help screen.",
 "Typing `frobby help' displays a list of the available actions.\n"
 "Typing `frobby help ACTION' displays a detailed description of that "
 "action.\n\n"
 "As an example, typing `frobby help irrdecom' will yield detailed "
 "information\n"
 "about the irrdecom action.",
 true) {
}

void HelpAction::obtainParameters(vector<Parameter*>& parameters) {
}

void HelpAction::processNonParameter(const char* str) {
  ASSERT(_topic == "");

  _topic = str;

  if (_topic != "io") {
	vector<string> names;
	Action::addNamesWithPrefix(str, names);
	if (names.empty())
	  reportError("Unknown help topic \"" + _topic + "\".");
  }
}

void HelpAction::displayActionHelp(Action* action) {
  fprintf(stderr, "Displaying information on action: %s\n\n%s\n",
		  action->getName(), action->getDescription());

  vector<Parameter*> parameters;
  action->obtainParameters(parameters);

  if (!parameters.empty()) {
    fprintf(stderr, "\nThe parameters accepted by %s are as follows.\n",
			action->getName());

    for (vector<Parameter*>::const_iterator it = parameters.begin();
		 it != parameters.end(); ++it) {

      string defaultValue;
      (*it)->getValue(defaultValue);

	  string rawDesc((*it)->getDescription());
	  string desc;
	  for (size_t i = 0; i < rawDesc.size(); ++i) {
		desc += rawDesc[i];
		if (rawDesc[i] == '\n' && i + 1 != rawDesc.size())
		  desc += "   "; // do proper indentation.";
	  }

      fprintf(stderr, "\n -%s %s   (default is %s)\n   %s\n",
			  (*it)->getName(), (*it)->getParameterName(),
			  defaultValue.c_str(), desc.c_str());
    }
  }
}

void HelpAction::displayIOHelp() {
  fputs("Displaying information on topic: io\n"
		"\n"
		"Frobby understands several file formats. These are not documented,\n"
		"but they are simple enough that seeing an example should be enough\n"
		"to figure them out. Getting an example is as simple as making\n"
		"Frobby produce output in that format.\n"
		"\n"
		"It is true of all the formats that white-space is insignificant,\n"
		"but other than that Frobby is quite fuzzy about how the input\n"
		"must look. E.g. a Macaulay 2 file containing a monomial ideal\n"
		"must start with \"R = \", so writing \"r = \" with a lower-case r\n"
		"is an error. To help with this, Frobby tries to say what is wrong\n"
		"if there is an error.\n"
		"\n"
		"If no input format is specified, Frobby will guess at the format,\n"
		"and it will guess correctly if there are no errors in the input.\n"
		"If no output format is specified, Frobby will use the same format\n"
		"for output as for input. If you want to force Frobby to use a\n"
		"specific format, use the -iformat and -oformat options. Using\n"
		"these with the transform action allows translation between formats.\n"
		"\n"
		"The formats available in Frobby and the types of data they\n"
		"support are as follows.\n\n", stderr);

  vector<string> names;
  IOHandler::addFormatNames(names);
  for (vector<string>::const_iterator name = names.begin();
	   name != names.end(); ++name) {
	auto_ptr<IOHandler> handler = IOHandler::createIOHandler(*name);
	ASSERT(handler.get() != 0);

	fprintf(stderr, "* The format %s: %s\n",
			handler->getName(),
			handler->getDescription());

	
	vector<const DataType*> types = DataType::getTypes();
	for (vector<const DataType*>::const_iterator typeIt = types.begin();
		 typeIt != types.end(); ++typeIt) {
	  const DataType& type = **typeIt;

	  bool input = handler->supportsInput(type);
	  bool output = handler->supportsOutput(type);

	  const char* formatStr = "";
	  if (input && output)
		formatStr = "  - supports input and output of %s.\n";
	  else if (input)
		formatStr = "  - supports input of %s.\n";
	  else if (output)
		formatStr = "  - supports output of %s.\n";

	  fprintf(stderr, formatStr, type.getName());
	}

	fputc('\n', stderr);
  }
}

void HelpAction::perform() {
  if (_topic != "") {
	if (_topic == "io")
	  displayIOHelp();
	else {
	  auto_ptr<Action> action(Action::createActionWithPrefix(_topic));
	  displayActionHelp(action.get());
	}

    return;
  }

  fprintf(stderr,
"Frobby version %s Copyright (C) 2007 Bjarke Hammersholt Roune\n"
"Frobby performs a number of computations related to monomial ideals. You\n"
"run it by typing `frobby ACTION', where ACTION is one of the following.\n\n",
	  constants::version);

  vector<string> names;
  Action::addNamesWithPrefix("", names);

  // Compute maximum name length to make descriptions line up.
  size_t maxNameLength = 0;
  for (vector<string>::const_iterator it = names.begin();
       it != names.end(); ++it)
    if (maxNameLength < it->size())
      maxNameLength = it->size();

  for (vector<string>::const_iterator it = names.begin();
       it != names.end(); ++it) {
	auto_ptr<Action> action(Action::createActionWithPrefix(*it));
    if (string("help") == action->getName() ||
		string("test") == action->getName())
      continue;

    size_t length = (string(action->getName())).size();
    fputc(' ', stderr);
    fputs(action->getName(), stderr);
    for (size_t i = length; i < maxNameLength; ++i)
      fputc(' ', stderr);
    fprintf(stderr, " - %s\n", action->getShortDescription());
  }

  fputs(
"\n"
"Type 'frobby help ACTION' to get more details on a specific action.\n"
"Note that all input and output is done via the standard streams.\n"
"Type 'frobby help io' for more information on input and output formats.\n"
"See www.broune.com for further information and new versions of Frobby.\n"
"\n"
"Frobby is free software and you are welcome to redistribute it under certain\n"
"conditions. Frobby comes with ABSOLUTELY NO WARRANTY. See the GNU General\n"
"Public License version 2.0 in the file COPYING for details.\n", stderr);
}

const char* HelpAction::staticGetName() {
  return "help";
}
