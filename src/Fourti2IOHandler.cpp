#include "stdinc.h"
#include "Fourti2IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "Term.h"
#include "TermTranslator.h"

#include <sstream>

// We have to store all the output before writing it out because we
// need to writer the number of terms at the top of the file.
class Fourti2IdealWriter : public IdealWriter {
public:
  Fourti2IdealWriter(FILE* file, const VarNames& names):
    IdealWriter(file, names),
	_termCount(0) {
  }

  Fourti2IdealWriter(FILE* file, const TermTranslator* translator):
    IdealWriter(file, translator, false),
	_termCount(0) {
  }

  virtual ~Fourti2IdealWriter() {
	fprintf(stdout, "%lu %lu\n%s",
			(unsigned long)_termCount,
			(unsigned long)_names.getVarCount(),
			_output.c_str());
  }

  virtual void consume(const vector<const char*>& term) {
	++_termCount;
	size_t varCount = term.size();
	for (size_t var = 0; var < varCount; ++var) {
	  const char* exp = term[var];
	  if (exp == 0)
		exp = "0";
	  if (var != 0)
		_output += ' ';
	  _output += exp;
	}
	_output += '\n';
  }

  virtual void consume(const vector<mpz_class>& term) {
	++_termCount;
	size_t varCount = term.size();
	for (size_t var = 0; var < varCount; ++var) {
	  if (var != 0)
		_output += ' ';
	  _output += term[var].get_str();
	}
	_output += '\n';
  }

  virtual void consume(const Term& term) {
	ASSERT(_translator != 0);

	++_termCount;
    size_t varCount = term.getVarCount();
    for (size_t var = 0; var < varCount; ++var) {
	  if (var != 0)
		_output += ' ';
	  const char* exp = _translator->getExponentString(var, term[var]);
	  if (exp == 0)
		exp = "0";
	  _output += exp;
	}
	_output += '\n';
  }

  void writeJustATerm(const Term& term) {
	ASSERT(_translator != 0);

    size_t varCount = term.getVarCount();
    for (size_t var = 0; var < varCount; ++var) {
	  fputc(' ', _file);
	  const char* exp = _translator->getExponentString(var, term[var]);
	  if (exp == 0)
		exp = "0";
	  fputs(exp, _file);
	}
  }

private:
  void writeTerm(const Term& term) {
  }

  string _output;
  size_t _termCount;
};

void Fourti2IOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  size_t termCount;
  size_t varCount;

  scanner.readInteger(termCount);
  scanner.readInteger(varCount);

  VarNames names(varCount);
  ideal.clearAndSetNames(names);

  ideal.reserve(termCount);
  for (size_t t = 0; t < termCount; ++t) {
	// Read a term
	ideal.newLastTerm();
	vector<mpz_class>& term = ideal.getLastTermRef();
	for (size_t var = 0; var < varCount; ++var)
	  scanner.readIntegerAndNegativeAsZero(term[var]);
  }
}

void Fourti2IOHandler::readIrreducibleDecomposition(Scanner& scanner,
													BigIdeal& decom) {
  fputs("ERROR: The 4ti2 format does not support decompositions.\n", stderr);
  exit(1);
}

void Fourti2IOHandler::readTerm(Scanner& in, const VarNames& names,
								vector<mpz_class>& term) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < names.getVarCount(); ++var)
	in.readIntegerAndNegativeAsZero(term[var]);
}

void Fourti2IOHandler::writeIdeal(FILE* out, const BigIdeal& ideal) {
  fprintf(out, "%lu %lu\n",
		  (unsigned long)ideal.getGeneratorCount(),
		  (unsigned long)ideal.getVarCount());

  for (size_t term = 0; term < ideal.getGeneratorCount(); ++term) {
	for (size_t var = 0; var < ideal[term].size(); ++var) {
	  if (var != 0)
		fputc(' ', out);
	  gmp_fprintf(out, "%Zd", ideal[term][var].get_mpz_t());
	}
	fputc('\n', out);
  }
}

IdealWriter* Fourti2IOHandler::createWriter
(FILE* file, const VarNames& names) const {
  displayWarning();
  return new Fourti2IdealWriter(file, names);
}

IdealWriter* Fourti2IOHandler::createWriter
(FILE* file, const TermTranslator* translator) const {
  displayWarning();
  return new Fourti2IdealWriter(file, translator);
}

const char* Fourti2IOHandler::getFormatName() const {
  return "4ti2";
}

void Fourti2IOHandler::displayWarning() const {
  fputs("NOTE: Using the 4ti2 format makes it necessary to store all of\n"
		"the output in memory before writing it out. This can increase memory\n"
		"consumption and decrease performance.\n", stderr);
}
