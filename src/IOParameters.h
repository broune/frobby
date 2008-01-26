#ifndef IO_PARAMETERS_GUARD
#define IO_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "StringParameter.h"

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

  void validateFormats() const;

 private:
  Type _type;

  StringParameter _inputFormat;
  StringParameter _outputFormat;
};

#endif
