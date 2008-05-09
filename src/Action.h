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
#ifndef ACTION_GUARD
#define ACTION_GUARD

#include "BoolParameter.h"
#include <vector>

class Parameter;

class Action {
 public:
  Action();
  virtual ~Action();

  virtual const char* getName() const = 0;
  virtual const char* getShortDescription() const = 0;
  virtual const char* getDescription() const = 0;
  virtual Action* createNew() const = 0;

  // processNonParameter() can be called at most once, and only if
  // acceptsNonParameter() returns true.
  virtual bool processNonParameter(const char* str);
  virtual bool acceptsNonParameter() const;

  virtual void obtainParameters(vector<Parameter*>& parameters) = 0;

  virtual void parseCommandLine(unsigned int tokenCount,
				const char** tokens);

  virtual void perform() = 0;

  // These methods are NOT thread safe.
  typedef vector<const Action*> ActionContainer;
  static const ActionContainer& getActions();
  static Action* createAction(const string& name);

 protected:
  BoolParameter _printActions;

 private:
  void processOption(const string& optionName,
		     const char** params,
		     unsigned int paramCount);

  static ActionContainer _actions;
  vector<Parameter*> _parameters;
};

#endif
