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
  Action(const char* name,
		 const char* shortDescription,
		 const char* description,
		 bool acceptsNonParameter);
  virtual ~Action();

  const char* getName() const;
  const char* getShortDescription() const;
  const char* getDescription() const;

  // processNonParameter() can be called at most once, and only if
  // acceptsNonParameter() returns true.
  bool acceptsNonParameter() const;
  virtual void processNonParameter(const char* str);

  virtual void obtainParameters(vector<Parameter*>& parameters) = 0;

  virtual void parseCommandLine(unsigned int tokenCount,
								const char** tokens);

  virtual void perform() = 0;

  /** Returns whether this action should be shown to the user by the
	help action.*/
  virtual bool displayAction() const;

  static void addNamesWithPrefix(const string& prefix,
								 vector<string>& names);
  static auto_ptr<Action> createActionWithPrefix(const string& prefix);

 protected:
  const char* _name;
  const char* _shortDescription;
  const char* _description;
  bool _acceptsNonParameter;

  BoolParameter _printActions;

 private:
  void processOption(const string& optionName,
					 const char** params,
					 unsigned int paramCount);

  vector<Parameter*> _parameters;
};

#endif
