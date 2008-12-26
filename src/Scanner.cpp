/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "Scanner.h"

#include "VarNames.h"
#include "IOHandler.h"
#include "error.h"
#include "FrobbyStringStream.h"

#include <limits>


//#define ENABLE_SCANNER_LOG

// This enables logging of what the scanner is reading and what it is
// being asked to do. This is very useful for debugging of code using
// Scanner, but the overhead is too high to include this when reading
// normally. Thus the inclusion of logging is controlled using the
// preprocessor.
#ifdef ENABLE_SCANNER_LOG
#define SCANNER_LOG(MSG) gmp_fprintf(stderr, MSG);
#define SCANNER_LOG1(MSG, PARAM) \
  gmp_fprintf(stderr, MSG, PARAM);
#define SCANNER_LOG2(MSG, PARAM1, PARAM2) \
  gmp_fprintf(stderr, MSG, PARAM1, PARAM2);
#else
#define SCANNER_LOG(MSG) ;
#define SCANNER_LOG1(MSG, PARAM) ;
#define SCANNER_LOG2(MSG, PARAM1, PARAM2) ;
#endif

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

auto_ptr<IOHandler> Scanner::createIOHandler() const {
  return IOHandler::createIOHandler(getFormat());
}

bool Scanner::match(char c) {
  SCANNER_LOG1("Matching the character '%c'.", c);

  eatWhite();
  if (c == peek()) {
    getChar();
	SCANNER_LOG(" Found match\n");
    return true;
  } else {
	SCANNER_LOG(" No match.\n");
    return false;
  }
}

bool Scanner::matchEOF() {
  SCANNER_LOG("Matching End-Of-File.\n");

  eatWhite();
  if (peek() == EOF) {
	SCANNER_LOG(" Found match\n");
	return true;
  } else {
	SCANNER_LOG(" No match.\n");
	return false;
  }
}

void Scanner::expect(char expected) {
  SCANNER_LOG1("Expecting the character '%c'.\n", expected);

  eatWhite();
  int got = getChar();
  if (got != expected) {
	FrobbyStringStream gotDescription;
	if (got == EOF)
	  gotDescription << "no more input";
	else
	  gotDescription << '\"' << static_cast<char>(got)<< '\"';

	string expectedStr;
	expectedStr += expected;
	reportErrorUnexpectedToken(expectedStr, gotDescription);
  }
}

unsigned int Scanner::getLineNumber() const {
  return _lineNumber;
}

void Scanner::expect(const char* str) {
  ASSERT(str != 0);
  SCANNER_LOG1("Expecting the string \"%s\".\n", str);

  eatWhite();

  const char* it = str;
  while (*it != '\0') {
	int character = getChar();
    if (*it == character) {
	  ++it;
	  continue;
	}

	// Read the rest of what is there to improve error message.
	// TODO: read at least one char in total even if not alnum.
	FrobbyStringStream got;
	if (character == EOF && it == str)
	  got << "no more input";
	else {
	  got << '\"' << string(str, it);
	  if (isalnum(character))
		got << static_cast<char>(character);
	  while (isalnum(peek()))
		got << static_cast<char>(getChar());
	  got << '\"';
	}

	reportErrorUnexpectedToken(str, got);
  }
}

void Scanner::expect(const string& str) {
  expect(str.c_str());
}

void Scanner::expectEOF() {
  SCANNER_LOG("Expecting End-Of-File.\n");

  // TODO: get this moved into the null format itself.
  if (_formatName == "null")
	return;

  eatWhite();
  if (getChar() != EOF)
	reportErrorUnexpectedToken("no more input", "");
}

void Scanner::expect(char a, char b) {
  SCANNER_LOG2("Expecting %c or %c.\n", a, b);

  if (!match(a) && !match(b)) {
	FrobbyStringStream err;
	err << a << " or " << b;
	reportErrorUnexpectedToken(err, "");
  }
}

size_t Scanner::readIntegerString() {
  eatWhite();

  ASSERT(_tmpStringCapacity > 1);

  if (peek() == '-' || peek() == '+')
    _tmpString[0] = static_cast<char>(getChar());
  else
	_tmpString[0] = '+';

  size_t size = 1;

  while (isdigit(peek())) {
    _tmpString[size] = static_cast<char>(getChar());
	++size;
	if (size == _tmpStringCapacity)
	  growTmpString();
  }
  _tmpString[size] = '\0';

  if (size == 1)
	reportErrorUnexpectedToken("an integer", "");

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
  SCANNER_LOG("Expecting arbitrary precision integer.");

  size_t size = readIntegerString();
  parseInteger(integer, size);

  SCANNER_LOG1(" Read %Zd.\n", integer.get_mpz_t());
}

void Scanner::readIntegerAndNegativeAsZero(mpz_class& integer) {
  SCANNER_LOG("Expecting arbitrary precision integer (negative to zero).");

  // Fast path for common case of reading a zero.
  if (peek() == '0') {
	getChar();
	if (!isdigit(peek())) {
	  integer = 0;

	  SCANNER_LOG(" Read 0.\n");
	  return;
	}
  }

  size_t size = readIntegerString();
  if (_tmpString[0] == '-')
	integer = 0;
  else
	parseInteger(integer, size);

  SCANNER_LOG1(" Read %Zd.\n", integer.get_mpz_t());
}

void Scanner::readSizeT(size_t& size) {
  SCANNER_LOG("Reading size_t non-negative integer as arbitrary precision.\n");

  readInteger(_integer);

  // Deal with different possibilities for how large size_t is.
  if (sizeof(size_t) == sizeof(unsigned int)) {
	if (!_integer.fits_uint_p()) {
	  FrobbyStringStream  errorMsg;
	  errorMsg << "expected non-negative integer of size at most "
			   << numeric_limits<unsigned int>::max()
			   << " but got " << _integer << '.';
	  reportSyntaxError(*this, errorMsg);
	}
	size = (unsigned int)_integer.get_ui();
  } else if (sizeof(size_t) == sizeof(unsigned long)) {
	if (!_integer.fits_ulong_p()) {
	  FrobbyStringStream errorMsg;
	  errorMsg << "expected non-negative integer of size at most "
			   << numeric_limits<unsigned long>::max()
			   << " but got " << _integer << '.';
	  reportSyntaxError(*this, errorMsg);
	}
	size = _integer.get_ui(); // returns an unsigned long despite the name.
  } else {
	FrobbyStringStream errorMsg;
	errorMsg << 
	  "Frobby does not work on this machine due to an "
	  "unexpected technical issue.\n"
	  "Please contact the developers of Frobby about this.\n"
	  "\n"
	  "Details that will be useful to the developers:\n"
	  " error location: Scanner::readSizeT\n"
	  " sizeof(size_t) = " << sizeof(size_t) << "\n"
	  " sizeof(unsigned int) = " << sizeof(unsigned int) << "\n"
	  " sizeof(unsigned long) = " << sizeof(unsigned long) << "\n";
	reportInternalError(errorMsg);
  }
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
  SCANNER_LOG("Expecting identifier.");

  eatWhite();
  if (!isalpha(peek()))
	reportErrorUnexpectedToken("an identifier", "");

  ASSERT(_tmpStringCapacity > 0);

  size_t size = 0;
  while (isalnum(peek())) {
    _tmpString[size] = static_cast<char>(getChar());
	++size;
	if (size == _tmpStringCapacity)
	  growTmpString();
  }
  _tmpString[size] = '\0';

  SCANNER_LOG1(" Read \"%s\".\n", _tmpString);
  return _tmpString;
}

size_t Scanner::readVariable(const VarNames& names) {
  SCANNER_LOG("Reading variable name as identifier.\n");

  const char* name = readIdentifier();
  size_t var = names.getIndex(name);
  if (var == VarNames::getInvalidIndex()) {
	FrobbyStringStream errorMsg;
	errorMsg << "Unknown variable \"" << name << "\". Maybe you forgot a *.";
	reportSyntaxError(*this, errorMsg);
  }
  return var;
}

bool Scanner::peekIdentifier() {
  eatWhite();

  SCANNER_LOG1("Peeking for identifier. Identifier %s.\n",
			   isalpha(peek()) ? "found" : "not found");

  return isalpha(peek());
}

bool Scanner::peekWhite() {
  SCANNER_LOG1("Peeking for space. Space %s.\n",
			   isspace(peek()) ? "found" : "not found");

  return isspace(peek());
}

bool Scanner::peek(char character) {
  eatWhite();

  SCANNER_LOG2("Peeking for character %c. Character %s.\n",
			   character, peek() == character ? "found" : "not found");

  return peek() == character;
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

void Scanner::reportErrorUnexpectedToken
(const string& expected, const string& got) {
  FrobbyStringStream errorMsg;
  errorMsg << "Expected " << expected;
  if (got != "")
	errorMsg << ", but got " << got;
  errorMsg << '.';
  reportSyntaxError(*this, errorMsg);
}

void Scanner::eatWhite() {
  while (isspace(peek()))
    getChar();
}
