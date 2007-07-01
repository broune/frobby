#include "stdinc.h"
#include "Lexer.h"

Lexer::Lexer(istream& in):
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
  eatWhite();

  char c = peek();
  if (c == '-' || c == '+')
    getChar();
    
  eatWhite();
  if (!isdigit(peek()))
    error("an integer");
    
  _in >> integer;
  if (c == '-')
    integer *= -1; // TODO: look up more efficient way to swap sign
}

void Lexer::readInteger(unsigned int& i) {
  readInteger(_integer);

  if (!_integer.fits_uint_p()) {
    cerr << "ERROR: expected 32 bit unsigned integer but got " << _integer << "." << endl;
    exit(0);
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
  int c = _in.get();
  if (c == '\n')
    ++_lineNumber;
  return c;
}

int Lexer::peek() {
  return _in.peek();
}

void Lexer::error(const string& expected) {
  cerr << "ERROR: expected " << expected << " at line "
       << _lineNumber << '.' << endl;
  exit(1);
}

void Lexer::eatWhite() {
  while (isspace(peek()))
    getChar();
}
