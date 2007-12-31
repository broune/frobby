#ifndef SCANNER_GUARD
#define SCANNER_GUARD

class Scanner {
public:
  Scanner(FILE* in);

  bool match(char c);
  bool matchEOF();

  void expect(char expected);
  void expect(const char* str);
  void expect(const string& str);
  void expectEOF();

  void readInteger(mpz_class& integer);
  void readInteger(unsigned int& i);

  void readIdentifier(string& identifier);

  bool peekIdentifier();

  unsigned int getLineNumber() const;

  // Writes "ERROR (line ?):" to standard error
  void printError();

private:
  int getChar();

  int peek();

  void error(const string& expected);

  void eatWhite();

  mpz_class _integer;
  FILE* _in;
  unsigned long _lineNumber;
};

#endif
