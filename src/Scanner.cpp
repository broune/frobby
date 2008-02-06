#include "stdinc.h"
#include "Scanner.h"

#include "VarNames.h"
#include "IOHandler.h"

Scanner::Scanner(const string& formatName, FILE* in):
  _in(in),
  _lineNumber(1),
  _char(' '),
  _tmpString(new char[16]),
  _tmpStringCapacity(16),
  _formatName(formatName) {
}

Scanner::~Scanner() {
  delete[] _tmpString;
}

const string& Scanner::getFormat() const {
  return _formatName;
}

void Scanner::setFormat(const string& format) {
  _formatName = format;
}

IOHandler* Scanner::getIOHandler() const {
  return IOHandler::getIOHandler(getFormat());
}

bool Scanner::match(char c) {
  eatWhite();
  if (c == peek()) {
    getChar();
    return true;
  } else
    return false;
}

bool Scanner::matchEOF() {
  eatWhite();
  return peek() == EOF;
}

void Scanner::expect(char expected) {
  eatWhite();
  if (getChar() != expected) {
    string str;
    str += expected;
    error(str);
  }
}

unsigned int Scanner::getLineNumber() const {
  return _lineNumber;
}

void Scanner::printError() {
  if (_formatName != "")
	fprintf(stderr, "ERROR (format %s, line %lu): ",
			_formatName.c_str(), _lineNumber);
  else
	fprintf(stderr, "ERROR (line %lu): ",_lineNumber);
}

void Scanner::expect(const char* str) {
  eatWhite();

  const char* it = str;
  while (*it != '\0') {
    if (*it != getChar())
      error(str);
    ++it;
  }
}

void Scanner::expect(const string& str) {
  expect(str.c_str());
}

void Scanner::expectEOF() {
  eatWhite();
  if (getChar() != EOF)
    error("end of input");
}

size_t Scanner::readIntegerString() {
  eatWhite();

  ASSERT(_tmpStringCapacity > 1);

  if (peek() == '-' || peek() == '+')
	_tmpString[0] = getChar();
  else
	_tmpString[0] = '+';

  size_t size = 1;

  while (isdigit(peek())) {
	_tmpString[size] = getChar();
	++size;
	if (size == _tmpStringCapacity)
	  growTmpString();
  }
  _tmpString[size] = '\0';

  if (size == 0)
	error("an integer");

  return size;
}

void Scanner::parseInteger(mpz_class& integer, size_t size) {
  // This code has a fast path for small integers and a slower path
  // for longer integers. The largest number representable in 32 bits
  // has 10 digits in base 10 and 1 char for the sign. If the number
  // we are reading has less than 10 digits, then we calculate it
  // directly without consulting GMP, which is faster.

  if (size < 10) {
	signed long l = 0;
	for (size_t i = 1; i < size; ++i)
	  l = 10 * l + (_tmpString[i] - '0');
	if (_tmpString[0] == '-')
	  l = -l;
	integer = l;
  } else {
	// For whatever reason mp_set_str does not support a + as the
	// first character.
	mpz_set_str(integer.get_mpz_t(), _tmpString + (_tmpString[0] != '-'), 10);
  }
}

void Scanner::readInteger(mpz_class& integer) {
  size_t size = readIntegerString();
  parseInteger(integer, size);
}

void Scanner::readIntegerAndNegativeAsZero(mpz_class& integer) {
  // Fast path for common case of reading a zero.
  if (peek() == '0') {
	getChar();
	if (!isdigit(peek())) {
	  integer = 0;
	  return;
	}
  }

  size_t size = readIntegerString();
  if (_tmpString[0] == '-')
	integer = 0;
  else
	parseInteger(integer, size);
}

void Scanner::readInteger(unsigned int& i) {
  readInteger(_integer);

  if (!_integer.fits_uint_p()) {
	printError();
    gmp_fprintf(stderr,
				"expected non-negative integer less than 2^32-1 but got %Zd.\n",
				_integer.get_mpz_t());
    exit(1);
  }
  i = _integer.get_ui();
}

void Scanner::growTmpString() {
  ASSERT(_tmpStringCapacity > 0);
  size_t newCapacity = _tmpStringCapacity * 2;
  char* str = new char[newCapacity];
  for (size_t i = 0; i < _tmpStringCapacity; ++i)
	str[i] = _tmpString[i];
  delete[] _tmpString;

  _tmpString = str;
  _tmpStringCapacity = newCapacity;
}

const char* Scanner::readIdentifier() {
  eatWhite();
  if (!isalpha(peek()))
	error("an identifier");

  ASSERT(_tmpStringCapacity > 0);

  size_t size = 0;
  while (isalnum(peek())) {
	_tmpString[size] = getChar();
	++size;
	if (size == _tmpStringCapacity)
	  growTmpString();
  }
  _tmpString[size] = '\0';

  return _tmpString;
}

size_t Scanner::readVariable(const VarNames& names) {
  const char* name = readIdentifier();
  size_t var = names.getIndex(name);
  if (var == VarNames::UNKNOWN) {
	printError();
    fprintf(stderr, "Unknown variable \"%s\". Maybe you forgot a *.\n", name);
    exit(1);
  }
  return var;
}

bool Scanner::peekIdentifier() {
  eatWhite();
  return isalpha(peek());
}

int Scanner::getChar() {
  if (_char == '\n')
    ++_lineNumber;
  int oldChar = _char;
  _char = getc(_in);
  return oldChar;
}

int Scanner::peek() {
  return _char;
}

void Scanner::error(const string& expected) {
  printError();
  fprintf(stderr, "Expected %s.\n", expected.c_str());
  exit(1);
}

void Scanner::eatWhite() {
  while (isspace(peek()))
    getChar();
}
