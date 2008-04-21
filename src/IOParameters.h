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
