#include "stdinc.h"
#include "io.h"

#include "Lexer.h"
#include "BigIdeal.h"
#include "VarNames.h"

#include "newMonosIO.h"
#include "monosIO.h"
#include "macaulay2IO.h"

#include <fstream>

IOHandler::IOHandlerContainer IOHandler::_ioHandlers;

IOHandler::~IOHandler() {
}

void IOHandler::writeIdeal(ostream& out, const BigIdeal& ideal) {
  startWritingIdeal(out, ideal.getNames());
  for (size_t i = 0; i < ideal.getGeneratorCount(); ++i)
    writeGeneratorOfIdeal(out, ideal[i], ideal.getNames());
  doneWritingIdeal(out);
}

void IOHandler::notImplemented(const char* operation) {
  cerr << "Format " << getFormatName()
       << " does not implement "
       << operation << endl;
  exit(0);
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
  if (var == VarNames::UNKNOWN) {
    cerr << "ERROR: Unknown variable \"" << varName << "\". Maybe you forgot a *.\n";
    exit(0);
  }
  
  if (lexer.match('^')) {
    lexer.readInteger(power);
    if (power <= 0) {
      cout << "ERROR: Expected positive integer as exponent but got " << power << '.' << endl;
      exit(0);
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

    // We need a handler for 4ti2
  }

  return _ioHandlers;
}

IOHandler* IOHandler::createIOHandler(const string& name) {
  getIOHandlers();
  for (IOHandlerContainer::const_iterator it = _ioHandlers.begin();
       it != _ioHandlers.end(); ++it) {
    if (name == (*it)->getFormatName())
      return (*it)->createNew();
  }

  return 0;
}

bool fileExists(const string& filename) {
  ifstream in(filename.c_str());
  in.close();
  return !in.fail();
}

void deleteFile(const string& filename) {
  system(("rm -f " + filename).c_str());
}    

void readFrobeniusInstance(istream& in, vector<mpz_class>& numbers) {
  numbers.clear();

  string number;
  while (in >> number) {
    for (unsigned int i = 0; i < number.size(); ++i) {
      if (!('0' <= number[i] && number[i] <= '9')) {
	cerr << "ERROR: Encountered character '"
	     << number[i] <<
	  "' while reading Frobenius instance." << endl;
	exit(0);
      }
    }

    // number cannot represent a negative number as '-' is not
    // valid in the input and would have been caught above.
    mpz_class n(number);
    if (n == 0 || n == 1) {
      cerr << "ERROR: Encountered the number "
	   << n << " while reading Frobenius instance." << endl;
      cerr << "Only integers strictly larger than 1 are valid." << endl;
      exit(0);
    }

    numbers.push_back(n);
  }

  if (numbers.empty()) {
    cerr << "ERROR: Encountered empty Frobenius instance." << endl;
    exit(0);
  }

  mpz_class gcd = numbers[0];
  for (unsigned int i = 1; i < numbers.size(); ++i)
    mpz_gcd(gcd.get_mpz_t(), gcd.get_mpz_t(), numbers[i].get_mpz_t());

  if (gcd != 1) {
    cerr << "ERROR: The numbers in the Frobenius instance are not "
	 << "relatively prime." << endl;
    exit(0);
  }
}
