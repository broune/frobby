#ifndef IO_HANDLER_GUARD
#define IO_HANDLER_GUARD

#include "TermConsumer.h"
#include "VarNames.h"

#include <vector>

class Term;
class Scanner;
class BigIdeal;
class TermTranslator;
class IdealWriter;
class Ideal;

class IOHandler {
 public:
  virtual ~IOHandler();

  virtual void readIdeal(Scanner& in, BigIdeal& ideal) = 0;
  virtual void readIrreducibleDecomposition(Scanner& in, BigIdeal& decom) = 0;

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);
  virtual void writeIdeal(FILE* out, const Ideal& ideal,
						  const TermTranslator* translator);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const = 0;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const = 0;

  virtual const char* getFormatName() const = 0;

  // Returns null if name is unknown.
  static IOHandler* getIOHandler(const string& name);

 protected:
  void readTerm(BigIdeal& ideal, Scanner& scanner);
  void readVarPower(vector<mpz_class>& term,
					const VarNames& names, Scanner& scanner);
};

class IdealWriter : public TermConsumer {
 public:
  IdealWriter();
  IdealWriter(FILE* file, const VarNames& names);
  IdealWriter(FILE* file, const TermTranslator* translator);
  virtual ~IdealWriter();

  virtual void consume(const vector<mpz_class>& term) = 0;
  virtual void consume(const Term& term) = 0;

 protected:
  static void writeTerm(const vector<const char*>& term, FILE* file);

  static void writeTerm(const Term& term,
			const TermTranslator* translator,
			FILE* file);

  static void writeTerm(const vector<mpz_class>& term,
			const VarNames& names,
			FILE* file);

  FILE* _file;
  VarNames _names;
  const TermTranslator* _translator;
};

void readFrobeniusInstance(Scanner& scanner, vector<mpz_class>& numbers);

#endif
