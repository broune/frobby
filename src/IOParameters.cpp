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
#include "IOParameters.h"

#include "IOFacade.h"
#include "MonosIOHandler.h"
#include "Scanner.h"

IOParameters::IOParameters(Type type):
  ParameterGroup("", ""),
  _type(type),

  _inputFormat
  ("iformat",
   "The supported input formats are monos, m2, 4ti2, null and newmonos.\n"
   "The special format autodetect instructs Frobby to figure the format\n"
   "out by itself, which it will do correctly if the input has no errors.",
   "autodetect"),

  _outputFormat
  ("oformat",
   type == OutputOnly ?
   "The supported output formats are monos, m2, 4ti2, null and newmonos." :
   "The output format. The additional format \"input\" means use input format.",
   type == OutputOnly ? 
   MonosIOHandler().getFormatName() : "input") {
  if (type != OutputOnly)
	addParameter(&_inputFormat);
  if (type != InputOnly)
	addParameter(&_outputFormat);
}

const string& IOParameters::getInputFormat() const {
  ASSERT(_type != OutputOnly);
  return _inputFormat;
}

const string& IOParameters::getOutputFormat() const {
  ASSERT(_type != InputOnly);

  if (_type != OutputOnly && _outputFormat.getValue() == "input")
	return _inputFormat;
  else
	return _outputFormat;
}

void IOParameters::autoDetectInputFormat(Scanner& in) {
  ASSERT(_type != OutputOnly);

  if (_inputFormat.getValue() == "autodetect") {
	// Get to the first non-whitespace character.
	in.eatWhite();
	int c = in.peek();

	// The first condition at each if is the correct one. The other ones
	// are attempts to catch easy mistakes.
	if (c == 'R' || c == 'I' || c == 'Z' || c == '=' || c == 'm' ||
		c == 'W' || c == 'q' || c == 'Q')
	  _inputFormat = "m2";
	else if (c == '(' || c == 'l' || c == ')')
	  _inputFormat = "newmonos";
	else if (isdigit(c) || c == '+' || c == '-')
	  _inputFormat = "4ti2";
	else // c shold be 'v' here, but we use monos as a fall back
	  _inputFormat = "monos";
  }

  if (in.getFormat() == "autodetect")
	in.setFormat(_inputFormat);
}

void IOParameters::validateFormats() const {
  IOFacade facade(false);

  if (_type != OutputOnly &&
	  !facade.isValidMonomialIdealFormat(getInputFormat())) {
    fprintf(stderr, "ERROR: Unknown input format \"%s\".\n",
			getInputFormat().c_str());
    exit(1);
  }

  if (_type != InputOnly &&
	  !facade.isValidMonomialIdealFormat(getOutputFormat())) {
    fprintf(stderr, "ERROR: Unknown output format \"%s\".\n",
			getOutputFormat().c_str());
    exit(1);
  }
}
