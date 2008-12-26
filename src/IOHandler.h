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

class IOHandler {
 public:
  virtual ~IOHandler();

  // Read an ideal and its ambient ring.
  virtual void readIdeal(Scanner& in, BigIdeal& ideal) = 0;

  // Read an ideal and possibly its ambient ring. If no ambient ring is
  // specified in the input, the format is allowed but not required to
  // use the passed-in ring.
  virtual void readIdeal(Scanner& in, BigIdeal& ideal,
						 const VarNames& names);

  // Read a list of ideals. In case of an empty list of ideals, the format is
  // allowed but not required to support reading a ring for the empty list.
  // This method is required to read the output of writeIdeals().
  //
  // The initial value of names is discarded and then names is set to the
  // last ring described as part of the list of ideals. In the case of an
  // empty list with a ring, this is the only way to obtain the ring. If
  // there are no rings specified, names is left unchanged.
  //
  // The vector ideals is required to be empty.
  virtual void readIdeals(Scanner& in,
						  vector<BigIdeal*>& ideals,
						  VarNames& names);

  virtual void readTerm(Scanner& in, const VarNames& names,
						vector<mpz_class>& term);
  virtual void readPolynomial(Scanner& in, BigPolynomial& polynomial);

  // Writes ideal to out. For format is allowed but not required to omit
  // a description of the ring if defineNewRing is false.
  virtual void writeIdeal(const BigIdeal& ideal,
						  bool defineNewRing,
						  FILE* out);

  // Writes ideals to out. If ideals is empty, the format is allowed to write
  // just the ring, or to discard the information about which ring is used.
  virtual void writeIdeals(const vector<BigIdeal*>& ideals,
						   const VarNames& names,
						   FILE* out);


  virtual void writePolynomial(const BigPolynomial& polynomial, FILE* out);
  virtual void writeTerm(const vector<mpz_class>& term,
						 const VarNames& names,
						 FILE* out) = 0;

  virtual bool hasMoreInput(Scanner& in) const;

  const char* getName() const;
  const char* getDescription() const;

  virtual TermConsumer* createIdealWriter
	(const TermTranslator* translator, FILE* out);

  virtual TermConsumer* createIrreducibleIdealWriter
	(const TermTranslator* translator, FILE* out);

  virtual CoefTermConsumer* createPolynomialWriter
	(const TermTranslator* translator, FILE* out);

  // Returns null if name is unknown.
  static auto_ptr<IOHandler> createIOHandler(const string& name);
  static void addFormatNames(vector<string>& names);

  enum DataType {
	None,
	MonomialIdeal,
	Polynomial,
	MonomialIdealList
  };

  bool supportsInput(DataType type) const;
  bool supportsOutput(DataType type) const;

  static const char* getDataTypeName(DataType type);
  static void addDataTypes(vector<DataType>& types);

 protected:
  // Output of polynomials.
  virtual void writePolynomialHeader(const VarNames& names, FILE* out);
  virtual void writePolynomialHeader(const VarNames& names,
									 size_t termCount,
									 FILE* out);
  virtual void writeTermOfPolynomial(const mpz_class& coef,
									 const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out);
  virtual void writeTermOfPolynomial(const mpz_class& coef,
									 const vector<mpz_class>& term,
									 const VarNames& names,
									 bool isFirst,
									 FILE* out);
  virtual void writePolynomialFooter(const VarNames& names,
									 bool wroteAnyGenerators,
									 FILE* out);

  // Output of monomial ideals. writeIdealHeader should be called
  // before starting output of a monomial ideal, and writeIdealFooter
  // should be called afterwards. The parameter defineNewRing specifies
  // whether the ring the ideal lies in needs to be written to the
  // file. E.g. it only needs to be specified once when writing a list of
  // ideals that all lie in the same ring.
  //
  // There are two overloads of writeIdealHeader because some formats
  // have to know the number of generators of the ideal before that ideal
  // can be written to the output. These formats are thus unable to implement
  // the overload that does not specify this number.
  virtual void writeIdealHeader(const VarNames& names,
								bool defineNewRing,
								FILE* out) = 0;
  virtual void writeIdealHeader(const VarNames& names,
								bool defineNewRing,
								size_t generatorCount,
								FILE* out);
  virtual void writeTermOfIdeal(const Term& term,
								const TermTranslator* translator,
								bool isFirst,
								FILE* out) = 0;
  virtual void writeTermOfIdeal(const vector<mpz_class> term,
								const VarNames& names,
								bool isFirst,
								FILE* out) = 0;
  virtual void writeIdealFooter(const VarNames& names,
								bool wroteAnyGenerators,
								FILE* out) = 0;

  void registerInput(DataType type);
  void registerOutput(DataType type);

  IOHandler(const char* formatName,
			const char* formatDescription,
			bool requiresSizeForIdealOutput);

  static void writeCoefTermProduct(const mpz_class& coef,
								   const Term& term,
								   const TermTranslator* translator,
								   bool hidePlus,
								   FILE* out);

  static void writeCoefTermProduct(const mpz_class& coef,
								   const vector<mpz_class>& term,
								   const VarNames& names,
								   bool hidePlus,
								   FILE* out);

  static void writeTermProduct(const Term& term,
							   const TermTranslator* translator,
							   FILE* out);

  static void writeTermProduct(const vector<mpz_class>& term,
							   const VarNames& names,
							   FILE* out);

  static void readTerm(BigIdeal& ideal, Scanner& in);
  static void readCoefTerm(BigPolynomial& polynomial,
						   bool firstTerm,
						   Scanner& in);
  static void readVarPower(vector<mpz_class>& term,
						   const VarNames& names, Scanner& in);

  vector<DataType> _supportedInputs;
  vector<DataType> _supportedOutputs;

  const char* _formatName;
  const char* _formatDescription;
  bool _requiresSizeForIdealOutput;

  friend class IdealWriter;
  friend class IrreducibleIdealWriter;
  friend class PolynomialWriter;
  friend class DelayedIdealWriter;
};

void readFrobeniusInstance(Scanner& in, vector<mpz_class>& numbers);

#endif
