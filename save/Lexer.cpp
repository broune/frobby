class Lexer {
  enum TokenType {
    INTEGER = 0,
    END_OF_FILE = 1,
    IDENTIFIER = 2,
    CHARACTER = 3
  } _tokenType;

public:
  Lexer(istream& in):
    _in(in) {
    nextToken();
  }

  bool match(char c) {
    if ((_tokenType == IDENTIFIER &&
	 _identifier.size() == 1 &&
	 _identifier[0] == c) ||

	(_tokenType == CHARACTER &&
	 _character == c)) {
      nextToken();
      return true;
    } else
      return false;
  }

  bool match(const char* str) {
    if (_tokenType == IDENTIFIER &&
	_identifier == str) {
      nextToken();
      return true;
    } else
      return false;
  }

  void expect(char expected) {
    if (!match(expected)) {
      cerr << "ERROR: expected '" << expected << "' but got ";
      printToken(cerr);
      cerr << '.' << endl;
      exit(0);
    }
  }

  void expectIdentifier(const char* str) {
    if (!match(str)) {
      cerr << "ERROR: expected \"" << str << "\" but got ";
      printToken(cerr);
      cerr << '.' << endl;
      exit(0);
    }
  }

  void expectEOF() {
    ensureType(END_OF_FILE);
  }

  void readInteger(mpz_class& integer) {
    ensureType(INTEGER);
    integer = _integer;

    nextToken();
  }

  void readInteger(unsigned int& i) {
    ensureType(INTEGER);

    if (!_integer.fits_uint_p()) {
      cerr << "ERROR: expected 32 bit unsigned integer but got " << _integer << "." << endl;
      exit(0);
    }
    i = _integer.get_ui();

    nextToken();
  }

  void readIdentifier(string& identifier) {
    ensureType(IDENTIFIER);
    identifier = _identifier;

    nextToken();
  }


private:
  int getChar() {
    return _in.get();
  }

  int peek() {
    return _in.peek();
  }

  void ensureType(TokenType type) {
    if (_tokenType != type) {
      cerr << "Expected ";
      printTokenType(cerr, type);
      cerr << " but got ";
      printToken(cerr);
      cerr << '.' << endl;
      exit(0);
    }
  }

  void eatWhite() {
    while (isspace(peek()))
      getChar();
  }

  void nextToken() {
    eatWhite();

    int c = peek();
    if (c == EOF)
      _tokenType = END_OF_FILE;
    else if (isdigit(c)) {
      _tokenType = INTEGER;
      _in >> _integer;
    } else if (c == '-' || c == '+') {
      getChar();
      eatWhite();
      if (isdigit(peek())) {
	_tokenType = INTEGER;
	_in >> _integer;
	if (c == '-')
	  _integer *= -1; // TODO: look up more efficient way to swap sign
      } else {
	_tokenType = CHARACTER;
	_character = c;
      }
    } else if (isalpha(c)) {
      _tokenType = IDENTIFIER;
      _identifier.clear();
      do
	_identifier += getChar();
      while (isalnum(peek()));
    } else {
      _tokenType = CHARACTER;
      _character = getChar();
    }
  }

  void printTokenType(ostream& out, TokenType type) {
    switch (type) {
    case END_OF_FILE:
      out << "end of file";
      break;
      
    case INTEGER:
      out << "an integer";
      break;

    case CHARACTER:
      out << "a character";
      break;

    case IDENTIFIER:
      out << "an identifier";
      break;

    default:
      ASSERT(false);
    }
  }

  void printToken(ostream& out) {
    switch (_tokenType) {
    case END_OF_FILE:
      out << "end of file";
      break;
      
    case INTEGER:
      out << _integer;
      break;

    case CHARACTER:
      out << '\'' << _character << '\'';
      break;

    case IDENTIFIER:
      out << '"' << _identifier << '"';
      break;

    default:
      ASSERT(false);
    }
  }

  mpz_class _integer;
  string _identifier;
  char _character;

  istream& _in;
};
