#include "stdinc.h"
#include "IOParameters.h"

#include "IOFacade.h"

IOParameters::IOParameters():
  ParameterGroup("", ""),

  _inputFormat
  ("iformat",
   "The supported input formats are monos, m2, 4ti2, null and newmonos.",
   "monos"),

  _outputFormat
  ("oformat",
   "The output format. The additional format \"auto\" means use input format.",
   "auto") {
  addParameter(&_inputFormat);
  addParameter(&_outputFormat);
}

const string& IOParameters::getInputFormat() const {
  return _inputFormat;
}

const string& IOParameters::getOutputFormat() const {
  if (_outputFormat.getValue() == "auto")
	return _inputFormat;
  else
	return _outputFormat;
}

void IOParameters::validateFormats() const {
  IOFacade facade(false);

  if (!facade.isValidMonomialIdealFormat(getInputFormat().c_str())) {
    fprintf(stderr, "ERROR: Unknown input format \"%s\".\n",
			getInputFormat().c_str());
    exit(1);
  }

  if (!facade.isValidMonomialIdealFormat(getOutputFormat().c_str())) {
    fprintf(stderr, "ERROR: Unknown output format \"%s\".\n",
			getOutputFormat().c_str());
    exit(1);
  }
}
