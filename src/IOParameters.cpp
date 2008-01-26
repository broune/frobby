#include "stdinc.h"
#include "IOParameters.h"

#include "IOFacade.h"
#include "MonosIOHandler.h"

IOParameters::IOParameters(Type type):
  ParameterGroup("", ""),
  _type(type),

  _inputFormat
  ("iformat",
   "The supported input formats are monos, m2, 4ti2, null and newmonos.",
   MonosIOHandler().getFormatName()),

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
  return _inputFormat;
}

const string& IOParameters::getOutputFormat() const {
  if (_type != OutputOnly && _outputFormat.getValue() == "auto")
	return _inputFormat;
  else
	return _outputFormat;
}

void IOParameters::validateFormats() const {
  IOFacade facade(false);

  if (!facade.isValidMonomialIdealFormat(getInputFormat())) {
    fprintf(stderr, "ERROR: Unknown input format \"%s\".\n",
			getInputFormat().c_str());
    exit(1);
  }

  if (!facade.isValidMonomialIdealFormat(getOutputFormat())) {
    fprintf(stderr, "ERROR: Unknown output format \"%s\".\n",
			getOutputFormat().c_str());
    exit(1);
  }
}
