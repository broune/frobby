#include "stdinc.h"
#include "HelpAction.h"

#include "Parameter.h"

HelpAction::HelpAction():
  _topic(0) {
}

const char* HelpAction::getName() const {
  return "help";
}

const char* HelpAction::getShortDescription() const {
  return "Display this help screen.";
}

const char* HelpAction::getDescription() const {
  return
"Typing `frobby help' displays a list of the available actions.\n"
"Typing `frobby help ACTION' displays a detailed description of that action.\n"
"\n"
"As an example, typing `frobby help irrdecom' will yield detailed information\n"
  "about the irrdecom action.";
}

Action* HelpAction::createNew() const {
  return new HelpAction();
}

void HelpAction::obtainParameters(vector<Parameter*>& parameters) {
}

bool HelpAction::acceptsNonParameter() const {
  return true;
}

bool HelpAction::processNonParameter(const char* str) {
  ASSERT(_topic == 0);

  _topic = Action::createAction(str);
  if (_topic == 0) {
    cerr << "ERROR: Unknown help topic \"" << str << "\"." << endl;
    return false;
  }

  return true;
}

void HelpAction::displayTopic() {
  fprintf(stdout, "Action: %s\n\n%s\n",
	  _topic->getName(), _topic->getDescription());

  vector<Parameter*> parameters;
  _topic->obtainParameters(parameters);

  if (!parameters.empty()) {
    fprintf(stdout, "\nThe parameters accepted by %s are as follows.\n",
	    _topic->getName());

    for (vector<Parameter*>::const_iterator it = parameters.begin();
	 it != parameters.end(); ++it) {
      string defaultValue;
      (*it)->getValue(defaultValue);
      fprintf(stdout, "\n -%s %s   (default is %s)\n   %s\n",
	      (*it)->getName(), (*it)->getParameterName(),
	      defaultValue.c_str(), (*it)->getDescription());
    }
  }
}

void HelpAction::perform() {
  if (_topic != 0) {
    displayTopic();
    return;
  }

  fprintf(stdout,
"Frobby version %s Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)\n"
"Frobby is free software and you are welcome to redistribute it under certain\n"
"conditions. Frobby comes with ABSOLUTELY NO WARRANTY. See the GNU General\n"
"Public License version 2.0 in the file COPYING for details.\n"
"\n"
"Frobby does a number of computations related to monomial ideals. You\n"
"run it by typing `frobby ACTION', where ACTION is one of the following.\n\n",
	  constants::version);

  const ActionContainer& actions = getActions();

  // Compute maximum name length to make descriptions line up.
  size_t maxNameLength = 0;
  for (ActionContainer::const_iterator it = actions.begin();
       it != actions.end(); ++it) {
    size_t length = (string((*it)->getName())).size();
    if (maxNameLength < length)
      maxNameLength = length;
  }

  for (ActionContainer::const_iterator it = actions.begin();
       it != actions.end(); ++it) {
    if (string("help") == (*it)->getName())
      continue;

    size_t length = (string((*it)->getName())).size();
    fputc(' ', stdout);
    fputs((*it)->getName(), stdout);
    for (size_t i = length; i < maxNameLength; ++i)
      fputc(' ', stdout);
    fprintf(stdout, " - %s\n", (*it)->getShortDescription());
  }

  fputs(
"\n"
"Type 'frobby help ACTION' to get more details on a specific action.\n"
"Note that all input and output is done via the standard streams.\n", stdout);
  fflush(stdout);
}
