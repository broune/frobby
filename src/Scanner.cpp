#include "stdinc.h"
#include "Scanner.h"

Scanner::Scanner(FILE* in):
  _in(in),
  _lineNumber(1) {
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
  fprintf(stderr, "ERROR (line %lu): ", _lineNumber);
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

void Scanner::readInteger(mpz_class& integer) {
  eatWhite();
  if (mpz_inp_str(integer.get_mpz_t(), _in, 10) == 0)
    error("an integer");
}

void Scanner::readInteger(unsigned int& i) {
  readInteger(_integer);

  if (!_integer.fits_uint_p()) {
    gmp_fprintf(stderr,
		"ERROR: expected 32 bit unsigned integer but got %Zd.\n",
		_integer.get_mpz_t());
    exit(1);
  }
  i = _integer.get_ui();
}

void Scanner::readIdentifier(string& identifier) {
  eatWhite();

  if (!isalpha(peek()))
    error("an identifier");

  identifier.clear();
  identifier += getChar();

  while (isalnum(peek()))
    identifier += getChar();
}

bool Scanner::peekIdentifier() {
  eatWhite();
  return isalpha(peek());
}

int Scanner::getChar() {
  int c = getc(_in);
  if (c == '\n')
    ++_lineNumber;
  return c;
}

int Scanner::peek() {
  int c = getc(_in);
  ungetc(c, _in);
  return c;
}

void Scanner::error(const string& expected) {
  fprintf(stderr, "ERROR: expected %s at line %lu.\n",
	  expected.c_str(), _lineNumber);
  exit(1);
}

void Scanner::eatWhite() {
  while (isspace(peek()))
    getChar();
}
