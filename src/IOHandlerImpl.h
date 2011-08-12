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
#include <vector>

class Term;
class Scanner;
class BigIdeal;
class VarNames;
class BigTermConsumer;
class DataType;
class CoefBigTermConsumer;
class SatBinomConsumer;
class TermTranslator;
class BigPolynomial;

namespace IO {
  /** This class contains a minimum level of functionality that makes
   it more convenient to derive from than IOHandler. */
  class IOHandlerImpl : public IOHandler {
  protected:
    IOHandlerImpl(const char* formatName,
                  const char* formatDescription);

    /** Specify that input of the argument type is supported. */
    void registerInput(const DataType& type);

    /** Specify that output of the argument type is supported. */
    void registerOutput(const DataType& type);

  private:
    // The following methods have implementations that merely report
    // an internal error. Make sure to override those you register
    // support for.
    virtual BigTermConsumer* doCreateIdealWriter(FILE* out) = 0;
    virtual CoefBigTermConsumer* doCreatePolynomialWriter(FILE* out);

    virtual void doReadTerm(Scanner& in, InputConsumer& term);
    virtual void doReadIdeal(Scanner& in, InputConsumer& consumer);
    virtual void doReadIdeals(Scanner& in, InputConsumer& consumer);
    virtual void doReadPolynomial(Scanner& in, CoefBigTermConsumer& consumer);
    virtual void doReadSatBinomIdeal(Scanner& in, SatBinomConsumer& consumer);

    // The following methods are implemented with the suggested
    // behavior
    virtual const char* doGetName() const;
    virtual const char* doGetDescription() const;
    virtual bool doSupportsInput(const DataType& type) const;
    virtual bool doSupportsOutput(const DataType& type) const;
    virtual bool doHasMoreInput(Scanner& in) const;

    vector<const DataType*> _supportedInputs;
    vector<const DataType*> _supportedOutputs;

    const char* _formatName;
    const char* _formatDescription;
  };

  void writeCoefTermProduct(const mpz_class& coef,
                            const Term& term,
                            const TermTranslator& translator,
                            bool hidePlus,
                            FILE* out);

  void writeCoefTermProduct(const mpz_class& coef,
                            const vector<mpz_class>& term,
                            const VarNames& names,
                            bool hidePlus,
                            FILE* out);

  void writeTermProduct(const Term& term,
                        const TermTranslator& translator,
                        FILE* out);

  void writeTermProduct(const vector<mpz_class>& term,
                        const VarNames& names,
                        FILE* out);

  void readTermProduct(Scanner& in,
                       const VarNames& names,
                       vector<mpz_class>& term);
  void readTermProduct(BigIdeal& ideal, Scanner& in);

  void readCoefTerm(BigPolynomial& polynomial,
                    bool firstTerm,
                    Scanner& in);

  void readCoefTerm(mpz_class& coef,
                    vector<mpz_class>& term,
                    const VarNames& names,
                    bool firstTerm,
                    Scanner& in);

  void readVarPower(vector<mpz_class>& term,
                    const VarNames& names,
                    Scanner& in);
}

#endif
