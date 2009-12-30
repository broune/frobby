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
#ifndef IO_HANDLER_IMPL_GUARD
#define IO_HANDLER_IMPL_GUARD

#include "IOHandler.h"

// @todo: preliminary
class IOHandlerImpl : public IOHandler {
 public:

 protected:
  IOHandlerImpl(const char* formatName,
				const char* formatDescription,
				bool requiresSizeForIdealOutput);

  /** For preserving ring information when writing an empty list of ideals. */
  virtual void writeRing(const VarNames& names, FILE* out) = 0;

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
  // TODO: this means that those derivatives are then not substitutable for
  // IOHandlers, so they should not be derivates of IOHandler. Fix this
  // removing this part of the IOHandler interface, and push it down into
  // two derivates, one with generatorCount and one without.
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
  virtual void writeTermOfIdeal(const vector<mpz_class>& term,
								const VarNames& names,
								bool isFirst,
								FILE* out) = 0;
  virtual void writeIdealFooter(const VarNames& names,
								bool wroteAnyGenerators,
								FILE* out) = 0;



  virtual void doReadTerm(Scanner& in, const VarNames& names,
						  vector<mpz_class>& term);


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

  using IOHandler::readTerm;
  static void readTerm(BigIdeal& ideal, Scanner& in);
  static void readCoefTerm(BigPolynomial& polynomial,
						   bool firstTerm,
						   Scanner& in);
  static void readCoefTerm
	(mpz_class& coef,
	 vector<mpz_class>& term,
	 const VarNames& names,
	 bool firstTerm,
	 Scanner& in);
  static void readVarPower(vector<mpz_class>& term,
						   const VarNames& names, Scanner& in);

  friend class IdealWriter;
  friend class PolynomialWriter;
  friend class DelayedIdealWriter;

  void registerInput(const DataType& type);
  void registerOutput(const DataType& type);

  virtual auto_ptr<BigTermConsumer> doCreateIdealWriter(FILE* out);
  virtual auto_ptr<CoefBigTermConsumer> doCreatePolynomialWriter(FILE* out);

 private:
  virtual const char* doGetName() const;
  virtual const char* doGetDescription() const;
  virtual bool doSupportsInput(const DataType& type) const;
  virtual bool doSupportsOutput(const DataType& type) const;
  virtual bool doHasMoreInput(Scanner& in) const;

  vector<const DataType*> _supportedInputs;
  vector<const DataType*> _supportedOutputs;

  const char* _formatName;
  const char* _formatDescription;
  bool _requiresSizeForIdealOutput;
};

#endif
