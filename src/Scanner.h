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

/** This class offers an input interface which is more convenient and
    for some purposes more efficient than dealing with a FILE*
    directly.  It keeps track of the current linenumber to report
    better error messages, so accurate error messages require that all
    input from the file be read through the same Scanner.

	All input methods whose documentation does not specifically say otherwise
	skip whitespace as defined by the standard isspace() method.

	There are three concepts for consuming input through a Scanner:
	
	Read X: Require an X to be in the input, and return what is read.

	Expect X: Require the exact value X to be in the input and skip past it.

	Match X: Return true if the exact value X is in the input, and in that case
    skip past it.
	
	If a requirement is not met, Scanner reports a syntax error using the
	functions in the error.h header.
*/
class Scanner {
public:
  Scanner(const string& formatName, FILE* in);
  ~Scanner();

  const string& getFormat() const;
  void setFormat(const string& format);
  auto_ptr<IOHandler> createIOHandler() const;

  /** Return true if the next character is c, and in that case skip
	  past it. */
  bool match(char c);

  /** Return true if no more input. */
  bool matchEOF();

  /** Require the next character to be equal to expected. This
	  character is skipped past. */
  void expect(char expected);

  /** Require the next character to be equal to a or b. This character
	  is skipped past. */
  void expect(char a, char b);

  /** Require the following characters to be equal to str. These
	  characters are skipped past. */
  void expect(const char* str);

  /** Require the following characters to be equal to str. These
	  characters are skipped past. */
  void expect(const string& str);

  /** Require that there is no more input. */
  void expectEOF();

  /** Read an arbitrary-precision integer. */
  void readInteger(mpz_class& integer);

  /** Read an integer and set it to zero if it is negative. This is
      more efficient because the sign can be detected before the
      integer is read. */
  void readIntegerAndNegativeAsZero(mpz_class& integer);

  /** Reads a size_t, where the representable range of that type
	  determines when the number is too big. The number is required to
	  be non-negative. */
  void readSizeT(size_t& size);

  /** The returned string is only valid until the next method on this
	  object gets called. */
  const char* readIdentifier();

  /** Reads an identifier and returns the index of that identifier as
	  a variable in names. */
  size_t readVariable(const VarNames& names);

  /** Skips whitespace and returns true if the next token is an
	  identifier. */
  bool peekIdentifier();

  /** Returns true if the next character is whitespace. Does not,
	  obviously, skip whitespace. */
  bool peekWhite();

  /** Skips whitespace and returns true if the next character is equal
	  to the parameter(s). */
  bool peek(char character);

  /** Returns the number of newlines seen. Does not skip
	  whitespace. */
  unsigned int getLineNumber() const;

  /** Returns the next character or EOF. Does not skip whitespace. */
  int peek();

  /** Reads past any whitespace, where whitespace is defined by the
	  standard function isspace(). */
  void eatWhite();

private:
  /** Returns the size of the string. */
  size_t readIntegerString();
  void parseInteger(mpz_class& integer, size_t size);

  int getChar();

  void reportErrorUnexpectedToken(const string& expected, const string& got);

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
