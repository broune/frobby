#ifndef IO_GUARD
#define IO_GUARD

#include "TermConsumer.h"
#include "VarNames.h"

#include <vector>
#include <sstream>

class Lexer;
class BigIdeal;
class VarNames;

class IdealWriter {
 public:
  virtual ~IdealWriter() {};

  virtual void consume(const vector<const char*>& term) = 0;
  virtual void consume(const vector<mpz_class>& term) {};

 protected:
  static void writeTerm(const vector<const char*>& generator, FILE* file) {
    char separator = ' ';
    size_t varCount = generator.size();
    for (size_t j = 0; j < varCount; ++j) {
      const char* exp = generator[j];
      if (exp == 0)
	continue;

      putc(separator, file);
      fputs(exp, file);
      separator = '*';
    }

    if (separator == ' ')
      fputs(" 1", file);
  }

  static void writeTerm(const vector<mpz_class>& term,
			const VarNames& names,
			FILE* file) {
    char separator = ' ';
    size_t varCount = term.size();
    for (size_t j = 0; j < varCount; ++j) {
      if ((term[j]) == 0)
	continue;

      putc(separator, file);
      separator = '*';

      fputs(names.getName(j).c_str(), file);
      if ((term[j]) != 1) {
	putchar('^');
	stringstream o;
	o << term[j];
	fputs(o.str().c_str(), file);
      }
    }

    if (separator == ' ')
      fputs(" 1", file);
  }
};

class IOHandler {
 public:
  IOHandler();
  virtual ~IOHandler();

  virtual void readIdeal(FILE* in, BigIdeal& ideal) = 0;
  virtual void readIrreducibleDecomposition(FILE* in, BigIdeal& decom) = 0;

  void writeIdeal(FILE* out, const BigIdeal& ideal);

  virtual IdealWriter* createWriter(FILE* file,
				    const VarNames& names) const = 0;


  virtual const char* getFormatName() const = 0;

  typedef vector<IOHandler*> IOHandlerContainer;

  virtual IOHandler* createNew() const = 0;

  // TODO make these not create a new handler.
  static const IOHandlerContainer& getIOHandlers();
  static IOHandler* getIOHandler(const string& name);

 private:
  static IOHandlerContainer _ioHandlers;

 protected:
  void notImplemented(const char* operation);
  void readTerm(BigIdeal& ideal, Lexer& lexer);
  void readVarPower(int& var, mpz_class& power,
		    const VarNames& names, Lexer& lexer);
};

void readFrobeniusInstance(FILE* in, vector<mpz_class>& numbers);

#endif
