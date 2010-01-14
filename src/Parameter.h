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
#ifndef PARAMETER_GUARD
#define PARAMETER_GUARD

#include <string>
#include <utility>

/** @todo: rename to CliParam */
class Parameter {
 public:
  Parameter(const string& name, const string& description);
  virtual ~Parameter();

  const string& getName() const {return _name;}
  const string& getDescription() const {return _description;}
  string getArgumentType() const {return doGetArgumentType();}
  string getValueAsString() const {return doGetValueAsString();}

  void appendToDescription(const char* str);

  void processArguments(const char** args, size_t argCount);

 private:
  virtual string doGetArgumentType() const = 0;
  virtual string doGetValueAsString() const = 0;
  virtual pair<size_t, size_t> doGetArgumentCountRange() const = 0;
  virtual void doProcessArguments(const char** args, size_t argCount) = 0;

  void checkCorrectParameterCount(unsigned int from,
                                  unsigned int to,
                                  const char** params,
                                  unsigned int paramCount);

  string _name;
  string _description;
};

#endif
