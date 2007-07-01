#ifndef LEXER_GUARD
#define LEXER_GUARD

class Lexer {
  enum TokenType {
    INTEGER = 0,
    END_OF_FILE = 1,
    IDENTIFIER = 2,
    CHARACTER = 3
  } _tokenType;

public:
  Lexer(istream& in):
    _in(in),
    _lineNumber(1) {
  }

  bool match(char c) {
    eatWhite();
    if (c == peek()) {
      getChar();
      return true;
    } else
      return false;
  }

  void expect(char expected) {
    eatWhite();
    if (getChar() != expected) {
      string str;
      str += expected;
      error(str);
    }
  }

  void expect(const char* str) {
    eatWhite();

    const char* it = str;
    while (*it != '\0') {
      if (*it != getChar())
	error(str);
      ++it;
    }
  }

  void expect(const string& str) {
    expect(str.c_str());
  }

  void expectEOF() {
    eatWhite();
    if (getChar() != EOF)
      error("end of input");
  }

  void readInteger(mpz_class& integer) {
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

  void readInteger(unsigned int& i) {
    readInteger(_integer);

    if (!_integer.fits_uint_p()) {
      cerr << "ERROR: expected 32 bit unsigned integer but got " << _integer << "." << endl;
      exit(0);
    }
    i = _integer.get_ui();
  }

  void readIdentifier(string& identifier) {
    eatWhite();

    if (!isalpha(peek()))
      error("an identifier");

    identifier.clear();
    identifier += getChar();

    while (isalnum(peek()))
      identifier += getChar();
  }

private:
  int getChar() {
    int c = _in.get();
    if (c == '\n')
      ++_lineNumber;
    return c;
  }

  int peek() {
    return _in.peek();
  }

  void error(const string& expected) {
    cerr << "ERROR: expected " << expected << " at line "
	 << _lineNumber << '.' << endl;
    exit(0);
  }

  void eatWhite() {
    while (isspace(peek()))
      getChar();
  }

  mpz_class _integer;
  istream& _in;
  unsigned int _lineNumber;
};

#endif
