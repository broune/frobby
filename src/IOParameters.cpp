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
   "The output format. The additional format \"auto\" means use input format.",
   type == OutputOnly ? 
   MonosIOHandler().getFormatName() : "auto") {
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

  if (_type != OutputOnly && _outputFormat.getValue() == "auto")
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
	if (c == 'R' || c == 'I' || c == 'Z' || c == '=' || c == 'm')
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
