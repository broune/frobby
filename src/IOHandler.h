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
#ifndef IO_HANDLER_GUARD
#define IO_HANDLER_GUARD

#include <vector>

class Term;
class Scanner;
class BigIdeal;
class TermTranslator;
class CoefTermConsumer;
class TermConsumer;
class VarNames;
class BigPolynomial;
class BigTermConsumer;
class DataType;
class CoefBigTermConsumer;
class SatBinomConsumer;

/** An IOHandler implements input and output for some format in such a
 way that client code does not need to know which format is being
 used. An IOHandler need have no mutable state of its own. If it needs
 to track state, that state is either passed through each method call,
 or the IOHandler returns an object that takes care of further IO and
 which itself has state.

 @ingroup IO
*/
class IOHandler {
 public:
  virtual ~IOHandler();

  /** Read an ideal and feed it to the consumer. */
  void readIdeal(Scanner& in, BigTermConsumer& consumer);

  /** Read a number of ideals and feed them to the consumer. */
  void readIdeals(Scanner& in, BigTermConsumer& consumer);

  void readTerm
	(Scanner& in, const VarNames& names, vector<mpz_class>& term);

  void readPolynomial(Scanner& in, CoefBigTermConsumer& consumer);

  void readSatBinomIdeal(Scanner& in, SatBinomConsumer& consumer);

  void writeTerm
	(const vector<mpz_class>& term, const VarNames& names, FILE* out);

  bool hasMoreInput(Scanner& in) const;

  const char* getName() const;
  const char* getDescription() const;

  auto_ptr<BigTermConsumer> createIdealWriter(FILE* out);
  auto_ptr<CoefBigTermConsumer> createPolynomialWriter(FILE* out);

  bool supportsInput(const DataType& type) const;
  bool supportsOutput(const DataType& type) const;

 protected:
  virtual auto_ptr<BigTermConsumer> doCreateIdealWriter(FILE* out) = 0;
  virtual auto_ptr<CoefBigTermConsumer> doCreatePolynomialWriter(FILE* out) = 0;

 private:
  virtual const char* doGetName() const = 0;
  virtual const char* doGetDescription() const = 0;

  virtual void doReadIdeal(Scanner& in, BigTermConsumer& consumer) = 0;
  virtual void doReadIdeals(Scanner& in, BigTermConsumer& consumer) = 0;
  virtual void doReadTerm(Scanner& in, const VarNames& names,
						  vector<mpz_class>& term) = 0;
  virtual void doReadPolynomial(Scanner& in, CoefBigTermConsumer& consumer) = 0;
  virtual void doReadSatBinomIdeal(Scanner& in, SatBinomConsumer& consumer) = 0;
  virtual void doWriteTerm(const vector<mpz_class>& term,
						   const VarNames& names,
						   FILE* out) = 0;
  virtual bool doHasMoreInput(Scanner& in) const = 0;

  virtual bool doSupportsInput(const DataType& type) const = 0;
  virtual bool doSupportsOutput(const DataType& type) const = 0;
};

/** Returns an IOHandler for the format with the passed in
	name. Throws an UnknownFormatException for unknown formats. */
auto_ptr<IOHandler> createIOHandler(const string& name);

/** Add the name of each fomat to names. */
void getIOHandlerNames(vector<string>& names);

void readFrobeniusInstance(Scanner& in, vector<mpz_class>& numbers);

/** Using this string where the name of a format is expected indicates
	to guess the format based on what is being read.

	@todo Use this all over.
*/
extern const char* FormatNameIndicatingToGuessTheFormat;

/** Return the format of what in is reading based on the first
 non-whitespace character. */
string autoDetectFormat(Scanner& in);

#endif
