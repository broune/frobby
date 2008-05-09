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
#ifndef IO_PARAMETERS_GUARD
#define IO_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "StringParameter.h"

class Scanner;

class IOParameters : public ParameterGroup {
 public:
  enum Type {
	InputOnly,
	OutputOnly,
	InputAndOutput
  };

  IOParameters(Type type = InputAndOutput);

  const string& getInputFormat() const;
  const string& getOutputFormat() const;

  // If using the input format, this must be called before validating
  // the ideals, since "autodetect" is not a valid format other than
  // as a place holder for the auto detected format. If the format on
  // in is autodetect, it will (also) be set.
  void autoDetectInputFormat(Scanner& in);
  void validateFormats() const;

 private:
  Type _type;

  StringParameter _inputFormat;
  StringParameter _outputFormat;
};

#endif
