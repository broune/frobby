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

class Parameter {
 public:
  Parameter(const char* name, const char* description);
  virtual ~Parameter();

  const char* getName() const;
  const char* getDescription() const;
  virtual const char* getParameterName() const;

  virtual void getValue(string& str) const = 0;

  virtual bool process(const char** params, unsigned int paramCount);
  virtual void processParameters(const char** params, unsigned int paramCount) = 0;

protected:
  void checkCorrectParameterCount(unsigned int from,
				  unsigned int to,
				  const char** params,
				  unsigned int paramCount);

 private:
  string _name;
  string _description;
};

#endif
