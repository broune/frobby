#ifndef LEXER_GUARD
#define LEXER_GUARD

class Lexer {
public:
  Lexer(FILE* in);

  bool match(char c);

  void expect(char expected);
  void expect(const char* str);
  void expect(const string& str);
  void expectEOF();

  void readInteger(mpz_class& integer);
  void readInteger(unsigned int& i);

  void readIdentifier(string& identifier);

  unsigned int getLineNumber() const;

private:
  int getChar();

  int peek();

  void error(const string& expected);

  void eatWhite();

  mpz_class _integer;
  FILE* _in;
  unsigned int _lineNumber;
};

#endif
