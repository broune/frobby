#ifndef IO_GUARD
#define IO_GUARD

#include "TermConsumer.h"
#include "VarNames.h"
#include "TermTranslator.h"
#include "Term.h"

#include <vector>

class Lexer;
class BigIdeal;
class VarNames;

class IdealWriter : public TermConsumer {
 public:
  IdealWriter() {}
  IdealWriter(FILE* file, const VarNames& names):
    _file(file),
    _names(names),
    _translator(0) {
  }
  IdealWriter(FILE* file, const TermTranslator* translator):
    _file(file),
    _names(translator->getNames()),
    _translator(translator) {
    _translator->makeStrings();
  }
  virtual ~IdealWriter() {};

  virtual void consume(const vector<const char*>& term) = 0;
  virtual void consume(const vector<mpz_class>& term) = 0;
  virtual void consume(const Term& term) = 0;

 protected:
  static void writeTerm(const vector<const char*>& term, FILE* file) {
    char separator = ' ';
    size_t varCount = term.size();
    for (size_t j = 0; j < varCount; ++j) {
      const char* exp = term[j];
      if (exp == 0)
	continue;

      putc(separator, file);
      separator = '*';

      fputs(exp, file);
    }

    if (separator == ' ')
      fputs(" 1", file);
  }

  static void writeTerm(const Term& term,
			const TermTranslator* translator,
			FILE* file) {
    char separator = ' ';
    size_t varCount = term.getVarCount();
    for (size_t j = 0; j < varCount; ++j) {
      const char* exp = translator->getExponentString(j, term[j]);
      if (exp == 0)
	continue;

      putc(separator, file);
      separator = '*';

      fputs(exp, file);
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
      if ((term[j]) != 1)
	gmp_printf("^%Zd", term[j].get_mpz_t());
    }

    if (separator == ' ')
      fputs(" 1", file);
  }

  FILE* _file;
  VarNames _names;
  const TermTranslator* _translator;
};

class IOHandler {
 public:
  IOHandler();
  virtual ~IOHandler();

  virtual void readIdeal(FILE* in, BigIdeal& ideal) = 0;
  virtual void readIrreducibleDecomposition(FILE* in, BigIdeal& decom) = 0;

  void writeIdeal(FILE* out, const BigIdeal& ideal);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const = 0;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const = 0;
  

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
