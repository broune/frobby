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
#include "stdinc.h"
#include "IOHandlerImpl.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "TermTranslator.h"
#include "Term.h"
#include "TermConsumer.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"
#include "Polynomial.h"
#include "BigPolynomial.h"
#include "FrobbyStringStream.h"
#include "BigTermConsumer.h"
#include "DataType.h"
#include "error.h"
#include "InputConsumer.h"
#include <algorithm>

IO::IOHandlerImpl::IOHandlerImpl(const char* formatName,
                                   const char* formatDescription):
  _formatName(formatName),
  _formatDescription(formatDescription) {
}

BigTermConsumer* IO::IOHandlerImpl::doCreateIdealWriter(FILE* out) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

CoefBigTermConsumer* IO::IOHandlerImpl::doCreatePolynomialWriter(FILE* out) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

bool IO::IOHandlerImpl::doSupportsInput(const DataType& type) const {
  return std::find(_supportedInputs.begin(), _supportedInputs.end(),
                   &type) != _supportedInputs.end();
}

bool IO::IOHandlerImpl::doSupportsOutput(const DataType& type) const {
  return std::find(_supportedOutputs.begin(), _supportedOutputs.end(),
                   &type) != _supportedOutputs.end();
}

void IO::IOHandlerImpl::registerInput(const DataType& type) {
  ASSERT(!type.isNull());
  ASSERT(!supportsInput(type));

  _supportedInputs.push_back(&type);
}

void IO::IOHandlerImpl::registerOutput(const DataType& type) {
  ASSERT(!type.isNull());
  ASSERT(!supportsOutput(type));

  _supportedOutputs.push_back(&type);
}

void IO::IOHandlerImpl::doReadTerm(Scanner& in, InputConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

void IO::IOHandlerImpl::doReadIdeal(Scanner& in, InputConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

void IO::IOHandlerImpl::doReadIdeals(Scanner& in, InputConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

void IO::IOHandlerImpl::doReadPolynomial(Scanner& in,
                                         CoefBigTermConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

void IO::IOHandlerImpl::doReadSatBinomIdeal(Scanner& in,
                                            SatBinomConsumer& consumer) {
  INTERNAL_ERROR_UNIMPLEMENTED();
}

bool IO::IOHandlerImpl::doHasMoreInput(Scanner& in) const {
  return !in.matchEOF();
}

const char* IO::IOHandlerImpl::doGetName() const {
  return _formatName;
}

const char* IO::IOHandlerImpl::doGetDescription() const {
  return _formatDescription;
}

void IO::readTermProduct(Scanner& in,
                         const VarNames& names,
                         vector<mpz_class>& term) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < term.size(); ++var)
    term[var] = 0;

  if (in.match('1'))
    return;

  do {
    readVarPower(term, names, in);
  } while (in.match('*'));
}

void IO::writeCoefTermProduct(const mpz_class& coef,
                              const Term& term,
                              const TermTranslator& translator,
                              bool hidePlus,
                              FILE* out) {
  if (coef >= 0 && !hidePlus)
    fputc('+', out);

  if (term.isIdentity()) {
    gmp_fprintf(out, "%Zd", coef.get_mpz_t());
    return;
  }

  if (coef == -1)
    fputc('-', out);
  else if (coef != 1)
    gmp_fprintf(out, "%Zd*", coef.get_mpz_t());

  writeTermProduct(term, translator, out);
}

void IO::writeCoefTermProduct(const mpz_class& coef,
                              const vector<mpz_class>& term,
                              const VarNames& names,
                              bool hidePlus,
                              FILE* out) {
  if (coef >= 0 && !hidePlus)
    fputc('+', out);

  bool isIdentity = true;
  for (size_t var = 0; var < term.size(); ++var)
    if (term[var] != 0)
      isIdentity = false;

  if (isIdentity) {
    gmp_fprintf(out, "%Zd", coef.get_mpz_t());
    return;
  }

  if (coef == -1)
    fputc('-', out);
  else if (coef != 1)
    gmp_fprintf(out, "%Zd*", coef.get_mpz_t());

  writeTermProduct(term, names, out);
}

void IO::writeTermProduct(const Term& term,
                          const TermTranslator& translator,
                          FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var) {
    const char* exp = translator.getVarExponentString(var, term[var]);
    if (exp == 0)
      continue;

    if (seenNonZero)
      putc('*', out);
    else
      seenNonZero = true;

    fputs(exp, out);
  }

  if (!seenNonZero)
    fputc('1', out);
}

void IO::writeTermProduct(const vector<mpz_class>& term,
                          const VarNames& names,
                          FILE* out) {
  bool seenNonZero = false;
  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var) {
    if (term[var] == 0)
      continue;

    if (seenNonZero)
      fputc('*', out);
    else
      seenNonZero = true;

    fputs(names.getName(var).c_str(), out);
    if ((term[var]) != 1) {
      fputc('^', out);
      mpz_out_str(out, 10, term[var].get_mpz_t());
    }
  }

  if (!seenNonZero)
    fputc('1', out);
}

void IO::readTermProduct(BigIdeal& ideal, Scanner& in) {
  ideal.newLastTerm();

  if (in.match('1'))
    return;

  do {
    readVarPower(ideal.getLastTermRef(), ideal.getNames(), in);
  } while (in.match('*'));
}

void IO::readCoefTerm(mpz_class& coef,
                      vector<mpz_class>& term,
                      const VarNames& names,
                      bool firstTerm,
                      Scanner& in) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < term.size(); ++var)
    term[var] = 0;

  bool positive = true;
  if (!firstTerm && in.match('+'))
    positive = !in.match('-');
  else if (in.match('-'))
    positive = false;
  else if (!firstTerm) {
    in.expect('+');
    return;
  }
  if (in.match('+') || in.match('-'))
    reportSyntaxError(in, "Too many adjacent signs.");

  if (in.peekIdentifier()) {
    coef = 1;
    readVarPower(term, names, in);
  } else
    in.readInteger(coef);

  while (in.match('*'))
    readVarPower(term, names, in);

  if (!positive)
    coef = -coef;
}

void IO::readCoefTerm(BigPolynomial& polynomial,
                      bool firstTerm,
                      Scanner& in) {
  polynomial.newLastTerm();
  mpz_class& coef = polynomial.getLastCoef();
  vector<mpz_class>& term = polynomial.getLastTerm();

  readCoefTerm(coef, term, polynomial.getNames(), firstTerm, in);
}

void IO::readVarPower(vector<mpz_class>& term,
                      const VarNames& names,
                      Scanner& in) {
  size_t var = in.readVariable(names);

  if (term[var] != 0) {
    const string& name = names.getName(var);
    reportSyntaxError(in, "The variable " +
                      name + " appears more than once in monomial.");
  }

  if (in.match('^')) {
    in.readInteger(term[var]);
    if (term[var] <= 0) {
      FrobbyStringStream errorMsg;
      errorMsg << "Expected positive integer as exponent but got "
               << term[var] << ".";
      reportSyntaxError(in, errorMsg);
    }
  } else
    term[var] = 1;
}
