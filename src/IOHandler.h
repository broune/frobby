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

class IOHandler {
 public:
  virtual ~IOHandler();

  virtual void readIdeal(Scanner& in, BigIdeal& ideal) = 0;
  virtual void readIdeals(Scanner& in, vector<BigIdeal*> ideals);
  virtual void readIrreducibleDecomposition(Scanner& in, BigIdeal& decom) = 0;
  virtual void readTerm(Scanner& in, const VarNames& names,
						vector<mpz_class>& term);

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);

  virtual bool hasMoreInput(Scanner& scanner) const;

  const char* getName() const;
  const char* getDescription() const;

  virtual CoefTermConsumer* createCoefTermWriter
	(FILE* out, const TermTranslator* translator);

  virtual TermConsumer* createIdealWriter
	(TermTranslator* translator, FILE* out);

  // Returns null if name is unknown.
  static IOHandler* getIOHandler(const string& name);
  static const vector<IOHandler*>& getIOHandlers();

  // TODO: in development. E.g. should be protected and pure virtual.
  virtual void writeIdealHeader(const VarNames& names, FILE* out) {ASSERT(false);}
  virtual void writeIdealHeader(const VarNames& names,
								size_t generatorCount,
								FILE* out)
	{writeIdealHeader(names, out);}

  virtual void writeTermOfIdeal(const Term& term,
								const TermTranslator* translator,
								bool isFirst,
								FILE* out) {ASSERT(false);}

  virtual void writeTermOfIdeal(const vector<mpz_class> term,
								const VarNames& names,
								bool isFirst,
								FILE* out) {ASSERT(false);}

  virtual void writeIdealFooter(FILE* out) {ASSERT(false);}

  enum DataType {
	None,
	MonomialIdeal,
	Polynomial,
	MonomialIdealList
  };

  bool supportsInput(DataType type) const;
  bool supportsOutput(DataType type) const;

  static const char* getDataTypeName(DataType type);
  static const vector<DataType>& getDataTypes();

 protected:
  void registerInput(DataType type);
  void registerOutput(DataType type);

  IOHandler(const char* formatName,
			const char* formatDescription,
			bool requiresSizeForIdealOutput);

  static void writeTermProduct(const Term& term,
							   const TermTranslator* translator,
							   FILE* out);

  static void writeTermProduct(const vector<mpz_class>& term,
							   const VarNames& names,
							   FILE* out);

  void readTerm(BigIdeal& ideal, Scanner& scanner);
  void readVarPower(vector<mpz_class>& term,
					const VarNames& names, Scanner& scanner);

 private:
  vector<DataType> _supportedInputs;
  vector<DataType> _supportedOutputs;

  const char* _formatName;
  const char* _formatDescription;
  bool _requiresSizeForIdealOutput;
};

void readFrobeniusInstance(Scanner& scanner, vector<mpz_class>& numbers);

#endif
