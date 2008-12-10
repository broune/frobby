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
#ifndef SCANNER_GUARD
#define SCANNER_GUARD

class VarNames;
class IOHandler;

class Scanner {
public:
  Scanner(const string& formatName, FILE* in);
  ~Scanner();

  const string& getFormat() const;
  void setFormat(const string& format);
  auto_ptr<IOHandler> createIOHandler() const;

  bool match(char c);
  bool matchEOF();

  void expect(char expected);
  void expect(const char* str);
  void expect(const string& str);
  void expectEOF();

  void readInteger(mpz_class& integer);
  void readIntegerAndNegativeAsZero(mpz_class& integer);
  void readSizeT(size_t& size);

  // The returned object is only valid until the next method on this
  // object gets called.
  const char* readIdentifier();
  size_t readVariable(const VarNames& names);

  bool peekIdentifier();
  bool peekWhite();

  unsigned int getLineNumber() const;

  // Writes "ERROR (line ?):" to standard error
  void printError(const char* str = 0);

  int peek();
  void eatWhite();

private:
  // returns size
  size_t readIntegerString();
  void parseInteger(mpz_class& integer, size_t size);

  int getChar();

  void error(const string& expected);

  void growTmpString();

  mpz_class _integer;
  FILE* _in;
  unsigned long _lineNumber;
  int _char; // next character on stream

  char* _tmpString;
  size_t _tmpStringCapacity;

  string _formatName;
};

#endif
