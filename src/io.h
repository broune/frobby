#ifndef IO_GUARD
#define IO_GUARD

class Lexer;
class BigIdeal;
class VarNames;

class IOHandler {
public:
  IOHandler();
  virtual ~IOHandler();

  virtual void readIdeal(istream& in, BigIdeal& ideal) = 0;

  void writeIdeal(FILE* out, const BigIdeal& ideal);

  // Piecewise output of an ideal
  virtual void startWritingIdeal(FILE* out,
				 const VarNames& names) = 0;

  virtual void writeGeneratorOfIdeal(FILE* out,
				     const vector<mpz_class>& generator,
				     const VarNames& names);

  virtual void writeGeneratorOfIdeal(FILE* out,
				     const vector<const char*>& generator,
				     const VarNames& names);

  virtual void doneWritingIdeal(FILE* out) = 0;


  virtual void readIrreducibleDecomposition(istream& in,
					    BigIdeal& decom) = 0;

  virtual const char* getFormatName() const = 0;

  typedef vector<const IOHandler*> IOHandlerContainer;

  virtual IOHandler* createNew() const = 0;

  // These methods are NOT thread safe.
  static const IOHandlerContainer& getIOHandlers();
  static IOHandler* createIOHandler(const string& name);

 private:
  static IOHandlerContainer _ioHandlers;

 protected:
  void notImplemented(const char* operation);
  void readTerm(BigIdeal& ideal, Lexer& lexer);
  void readVarPower(int& var, mpz_class& power,
		    const VarNames& names, Lexer& lexer);

  void writeTerm(FILE* out,
		 const vector<mpz_class>& generator,
		 const VarNames& names);

  void writeTerm(FILE* out,
		 const vector<const char*>& generator,
		 const VarNames& names);

  bool _justStartedWritingIdeal;
};

bool fileExists(const string& filename);
void deleteFile(const string& filename);
void readFrobeniusInstance(istream& in, vector<mpz_class>& numbers);

#endif
