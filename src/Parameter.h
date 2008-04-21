#ifndef PARAMETER_GUARD
#define PARAMETER_GUARD

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
  const char* _name;
  const char* _description;
};

#endif
