#include "stdinc.h"
#include "IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "TermTranslator.h"
#include "Term.h"

#include "NewMonosIOHandler.h"
#include "MonosIOHandler.h"
#include "Macaulay2IOHandler.h"
#include "Fourti2IOHandler.h"
#include "NullIOHandler.h"

#include <cctype>

IdealWriter::IdealWriter() {
}

IdealWriter::IdealWriter(FILE* file, const VarNames& names):
  _file(file),
  _names(names),
  _translator(0) {
}

IdealWriter::IdealWriter(FILE* file, const TermTranslator* translator):
  _file(file),
  _names(translator->getNames()),
  _translator(translator) {
  _translator->makeStrings();
}

IdealWriter::~IdealWriter() {
}

void IdealWriter::writeTerm(const vector<const char*>& term, FILE* file) {
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

void IdealWriter::writeTerm(const Term& term,
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

void IdealWriter::writeTerm(const vector<mpz_class>& term,
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

IOHandler::~IOHandler() {
}

void IOHandler::writeIdeal(FILE* out, const BigIdeal& ideal) {
  IdealWriter* writer = createWriter(out, ideal.getNames());
  for (size_t i = 0; i < ideal.getGeneratorCount(); ++i)
    writer->consume(ideal[i]);
  delete writer;
}

void IOHandler::readTerm(BigIdeal& ideal, Scanner& scanner) {
  ideal.newLastTerm();

  if (scanner.match('1'))
    return;

  do {
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), scanner);
  } while (scanner.match('*'));
}

void IOHandler::readVarPower(vector<mpz_class>& term,
							 const VarNames& names, Scanner& scanner) {
  /*  static string varName;
  scanner.readIdentifier(varName);
  size_t var = names.getIndex(varName);
  if (var == VarNames::UNKNOWN) {
	scanner.printError();
    fprintf(stderr, "Unknown variable \"%s\". Maybe you forgot a *.\n",
			varName.c_str());
    exit(1);
	}*/
  size_t var = scanner.readVariable(names);

  if (term[var] != 0) {
	scanner.printError();
	fputs("A variable appears twice.\n", stderr);
	exit(1);
  }

  if (scanner.match('^')) {
    scanner.readInteger(term[var]);
    if (term[var] <= 0) {
	  scanner.printError();
      gmp_fprintf
		(stderr, "Expected positive integer as exponent but got %Zd.\n",
		 term[var].get_mpz_t());
      exit(1);
    }
  } else
    term[var] = 1;
}

IOHandler* IOHandler::getIOHandler(const string& name) {
  static MonosIOHandler monos;
  if (name == monos.getFormatName())
	return &monos;

  static Fourti2IOHandler fourti2;
  if (name == fourti2.getFormatName())
	return &fourti2;

  static NewMonosIOHandler newMonos;
  if (name == newMonos.getFormatName())
	return &newMonos;

  static Macaulay2IOHandler m2;
  if (name == m2.getFormatName())
	return &m2;

  static NullIOHandler nullHandler;
  if (name == nullHandler.getFormatName())
	return &nullHandler;

  return 0;
}

void readFrobeniusInstance(Scanner& scanner, vector<mpz_class>& numbers) {
  numbers.clear();

  string number;
  mpz_class n;
  while (!scanner.matchEOF()) {
	scanner.readInteger(n);

    if (n <= 1) {
	  scanner.printError();
      gmp_fprintf(stderr,
				  "Read the number %Zd while reading Frobenius instance.\n"
				  "Only integers strictly larger than 1 are valid.\n",
				  n.get_mpz_t());
      exit(1);
    }

    numbers.push_back(n);
  }

  if (numbers.empty()) {
	scanner.printError();
    fputs("Read empty Frobenius instance, which is not allowed.\n", stderr);
    exit(1);
  }

  mpz_class gcd = numbers[0];
  for (size_t i = 1; i < numbers.size(); ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), numbers[i].get_mpz_t());

  if (gcd != 1) {
    gmp_fprintf(stderr,
				"ERROR: The numbers in the Frobenius instance are not "
				"relatively prime.\nThey are all divisible by %Zd.\n",
				gcd.get_mpz_t());
    exit(1);
  }
}
