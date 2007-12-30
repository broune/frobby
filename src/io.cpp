#include "stdinc.h"
#include "io.h"

#include "Lexer.h"
#include "BigIdeal.h"
#include "TermTranslator.h"
#include "Term.h"

#include "newMonosIO.h"
#include "monosIO.h"
#include "macaulay2IO.h"

#include <fstream>
#include <cctype>
#include <sstream>

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

IOHandler::IOHandlerContainer IOHandler::_ioHandlers;

IOHandler::IOHandler() {
}

IOHandler::~IOHandler() {
}

void IOHandler::writeIdeal(FILE* out, const BigIdeal& ideal) {
  IdealWriter* writer = createWriter(out, ideal.getNames());
  for (size_t i = 0; i < ideal.getGeneratorCount(); ++i)
    writer->consume(ideal[i]);
  delete writer;
}

void IOHandler::notImplemented(const char* operation) {
  fprintf(stderr, "ERROR: Format %s does not implement %s.", 
	  getFormatName(), operation);
  exit(1);
}

void IOHandler::readTerm(BigIdeal& ideal, Lexer& lexer) {
  ideal.newLastTerm();

  if (lexer.match('1'))
    return;
  
  int var;
  mpz_class power;
  do {
    readVarPower(var, power, ideal.getNames(), lexer);
    ideal.getLastTermExponentRef(var) += power;
  } while (lexer.match('*'));
}

void IOHandler::readVarPower(int& var, mpz_class& power,
			     const VarNames& names, Lexer& lexer) {
  string varName;
  lexer.readIdentifier(varName);
  var = names.getIndex(varName);
  if ((size_t)var == VarNames::UNKNOWN) {
    fprintf(stderr, "ERROR: Unknown variable \"%s\". Maybe you forgot a *.\n",
	    varName.c_str());
    exit(1);
  }
  
  if (lexer.match('^')) {
    lexer.readInteger(power);
    if (power <= 0) {
      gmp_fprintf
	(stderr, "ERROR: Expected positive integer as exponent but got %Zd.\n",
	 power.get_mpz_t());
      exit(1);
    }
  } else
    power = 1;
}

const IOHandler::IOHandlerContainer& IOHandler::getIOHandlers() {
  if (_ioHandlers.empty()) {
    // This method uses static variables instead of new to avoid
    // spurious reports from memory leak detectors.

    static MonosIOHandler monos;
    _ioHandlers.push_back(&monos);

    static NewMonosIOHandler newMonos;
    _ioHandlers.push_back(&newMonos);

    static Macaulay2IOHandler m2;
    _ioHandlers.push_back(&m2);

    // TODO: We need a handler for 4ti2
  }

  return _ioHandlers;
}

IOHandler* IOHandler::getIOHandler(const string& name) {
  getIOHandlers();
  for (IOHandlerContainer::iterator it = _ioHandlers.begin();
       it != _ioHandlers.end(); ++it) {
    if (name == (*it)->getFormatName())
      return *it;
  }

  return 0;
}

bool getst(FILE* in, string& str) {
  str.clear();

  while (true) {
    int c = getc(in);
    if (isspace(c))
      continue;
    ungetc(c, in);
    break;
  }

  while (true) {
    int c = getc(in);
    if (isspace(c) || c == EOF) {
      ungetc(c, in);
      break;
    }
     str += c;
  }
  
   return !str.empty();
}

void readFrobeniusInstance(FILE* in, vector<mpz_class>& numbers) {
  numbers.clear();

  string number;
  while (getst(in, number)) {
    for (unsigned int i = 0; i < number.size(); ++i) {
      if (!('0' <= number[i] && number[i] <= '9')) {
	gmp_fprintf(stderr, "ERROR: Encountered character '%c' "
		    "while reading Frobenius instance.\n", number[i]);
	exit(1);
      }
    }

    // number cannot represent a negative number as '-' is not
    // valid in the input and would have been caught above.
    mpz_class n(number);
    if (n == 0 || n == 1) {
      gmp_fprintf
	(stderr, "ERROR: Encountered the number %Zd while reading "
	 "Frobenius instance.\n"
	 "Only integers strictly larger than 1 are valid.\n",
	 n.get_mpz_t());
      exit(1);
    }

    numbers.push_back(n);
  }

  if (numbers.empty()) {
    fputs("ERROR: Encountered empty Frobenius instance.\n", stderr);
    exit(1);
  }

  mpz_class gcd = numbers[0];
  for (unsigned int i = 1; i < numbers.size(); ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), numbers[i].get_mpz_t());

  if (gcd != 1) {
    fputs("ERROR: The numbers in the Frobenius instance are not "
	  "relatively prime.\n", stderr);
    exit(1);
  }
}
