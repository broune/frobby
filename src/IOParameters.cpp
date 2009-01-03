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
#include "error.h"
#include "FrobbyStringStream.h"
#include "DataType.h"

IOParameters::IOParameters(const DataType& input, const DataType& output):
  _inputType(input),
  _outputType(output),
  _inputFormat(0),
  _outputFormat(0) {

  string inputFormats;
  string outputFormats;

  string defaultOutput;
  if (!_inputType.isNull())
	defaultOutput = "input";

  vector<string> names;
  IOHandler::addFormatNames(names);
  for (vector<string>::const_iterator name = names.begin();
	   name != names.end(); ++name) {
	auto_ptr<IOHandler> handler = IOHandler::createIOHandler(*name);
	ASSERT(handler.get() != 0);

	if (handler->supportsInput(_inputType)) {
	  inputFormats += ' ';
	  inputFormats += handler->getName();
	}
	if (handler->supportsOutput(_outputType)) {
	  if (defaultOutput.empty())
		defaultOutput = handler->getName();
	  outputFormats += ' ';
	  outputFormats += handler->getName();
	}
  }

  if (!_inputType.isNull()) {
	string desc =
      "The format used to read the input. "
	  "This action supports the formats:\n " + inputFormats + ".\n"
	  "The format \"autodetect\" instructs Frobby to guess the format.\n"
	  "Type 'frobby help io' for more information on input formats.";

	_inputFormat.reset
	  (new StringParameter("iformat", desc.c_str(), "autodetect"));
	addParameter(_inputFormat.get());
  }

  if (!output.isNull()) {
	string desc =
	  "The format used to write the output. "
	  "This action supports the formats:\n " + outputFormats + ".\n";
	if (!_inputType.isNull()) {
	  desc +=
		"The format \"input\" instructs Frobby to use the input format.\n";
	}
	desc += "Type 'frobby help io' for more information on output formats.";

	_outputFormat.reset
	  (new StringParameter("oformat", desc.c_str(), defaultOutput));
	addParameter(_outputFormat.get());
  }
}

void IOParameters::setOutputFormat(const string& format) {
  ASSERT(!_inputType.isNull());
  ASSERT(_outputFormat.get() != 0);

  *_outputFormat = format;
}

const string& IOParameters::getInputFormat() const {
  ASSERT(!_inputType.isNull());
  ASSERT(_inputFormat.get() != 0);

  return *_inputFormat;
}

const string& IOParameters::getOutputFormat() const {
  ASSERT(!_outputType.isNull());
  ASSERT(_outputFormat.get() != 0);

  if (!_inputType.isNull() && _outputFormat->getValue() == "input") {
	ASSERT(_inputFormat.get() != 0);
	return *_inputFormat;
  }

  return *_outputFormat;
}

auto_ptr<IOHandler> IOParameters::createInputHandler() const {
  auto_ptr<IOHandler> handler(IOHandler::createIOHandler(getInputFormat()));
  ASSERT(handler.get() != 0);
  return handler;
}

auto_ptr<IOHandler> IOParameters::createOutputHandler() const {
  auto_ptr<IOHandler> handler(IOHandler::createIOHandler(getOutputFormat()));
  ASSERT(handler.get() != 0);
  return handler;
}

void IOParameters::autoDetectInputFormat(Scanner& in) {
  ASSERT(!_inputType.isNull());
  ASSERT(_inputFormat.get() != 0);

  if (_inputFormat->getValue() == "autodetect") {
	// Get to the first non-whitespace character.
	in.eatWhite();
	int c = in.peek();

	// The first condition at each if is the correct one. The other ones
	// are attempts to catch easy mistakes.
	if (c == 'R' || c == 'I' || c == 'Z' || c == '=' || c == 'm' ||
		c == 'W' || c == 'q' || c == 'Q')
	  *_inputFormat = "m2";
	else if (c == 'U' || c == 'u')
	  *_inputFormat = "cocoa4";
	else if (c == 'r')
	  *_inputFormat = "singular";
	else if (c == '(' || c == 'l' || c == ')')
	  *_inputFormat = "newmonos";
	else if (isdigit(c) || c == '+' || c == '-')
	  *_inputFormat = "4ti2";
	else if (c == 'v')
	  *_inputFormat = "monos";
	else
	  *_inputFormat = "m2"; // We use m2 as a fall-back
  }

  if (in.getFormat() == "autodetect")
	in.setFormat(*_inputFormat);
}

void IOParameters::validateFormats() const {
  IOFacade facade(false);

  if (!_inputType.isNull()) {
	auto_ptr<IOHandler> handler(IOHandler::createIOHandler(getInputFormat()));
	if (handler.get() == 0)
	  reportError("Unknown input format \"" + getInputFormat() + "\".");

	if (!handler->supportsInput(_inputType)) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "The "
			   << handler->getName()
			   << " format does not support input of "
			   << _inputType.getName()
			   << '.';
	  reportError(errorMsg);
	}
  }

  if (!_outputType.isNull()) {
	auto_ptr<IOHandler> handler(IOHandler::createIOHandler(getOutputFormat()));
	if (handler.get() == 0)
	  reportError("Unknown output format \"" + getOutputFormat() + "\".");

	if (!handler->supportsOutput(_outputType)) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "The "
			   << handler->getName()
			   << " format does not support output of "
			   << _outputType.getName()
			   << '.';
	  reportError(errorMsg);
	}
  }
}
