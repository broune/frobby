#ifndef IO_PARAMETERS_GUARD
#define IO_PARAMETERS_GUARD

#include "ParameterGroup.h"
#include "StringParameter.h"

class IOParameters : public ParameterGroup {
 public:
  IOParameters();

  const string& getInputFormat() const;
  const string& getOutputFormat() const;

  void validateFormats() const;

 private:
  StringParameter _inputFormat;
  StringParameter _outputFormat;
};

#endif
