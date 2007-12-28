#include "stdinc.h"
#include "Lexer.h"

Lexer::Lexer(FILE* in):
  _in(in),
  _lineNumber(1) {
}

bool Lexer::match(char c) {
  eatWhite();
  if (c == peek()) {
    getChar();
    return true;
  } else
    return false;
}

void Lexer::expect(char expected) {
  eatWhite();
  if (getChar() != expected) {
    string str;
    str += expected;
    error(str);
  }
}

unsigned int Lexer::getLineNumber() const {
  return _lineNumber;
}

void Lexer::expect(const char* str) {
  eatWhite();

  const char* it = str;
  while (*it != '\0') {
    if (*it != getChar())
      error(str);
    ++it;
  }
}

void Lexer::expect(const string& str) {
  expect(str.c_str());
}

void Lexer::expectEOF() {
  eatWhite();
  if (getChar() != EOF)
    error("end of input");
}

void Lexer::readInteger(mpz_class& integer) {
  if (mpz_inp_str(integer.get_mpz_t(), _in, 10) == 0)
    error("an integer");
}

void Lexer::readInteger(unsigned int& i) {
  readInteger(_integer);

  if (!_integer.fits_uint_p()) {
    gmp_fprintf(stderr,
		"ERROR: expected 32 bit unsigned integer but got %Zd.\n",
		_integer.get_mpz_t());
    exit(1);
  }
  i = _integer.get_ui();
}

void Lexer::readIdentifier(string& identifier) {
  eatWhite();

  if (!isalpha(peek()))
    error("an identifier");

  identifier.clear();
  identifier += getChar();

  while (isalnum(peek()))
    identifier += getChar();
}

int Lexer::getChar() {
  int c = getc(_in);
  if (c == '\n')
    ++_lineNumber;
  return c;
}

int Lexer::peek() {
  int c = getc(_in);
  ungetc(c, _in);
  return c;
}

void Lexer::error(const string& expected) {
  fprintf(stderr, "ERROR: expected %s at line %u.\n",
	  expected.c_str(), _lineNumber);
  exit(1);
}

void Lexer::eatWhite() {
  while (isspace(peek()))
    getChar();
}
