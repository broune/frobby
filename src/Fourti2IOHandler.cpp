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
#include "Fourti2IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "Term.h"
#include "TermTranslator.h"

Fourti2IOHandler::Fourti2IOHandler():
  IOHandler("4ti2", "Format used by the software package 4ti2.", true) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(Polynomial);
}

void Fourti2IOHandler::writePolynomialHeader(const VarNames& names,
											 size_t termCount,
											 FILE* out) {
  ASSERT(out != 0);

  fprintf(out, "%lu %lu\n",
		  (unsigned long)termCount,
		  (unsigned long)names.getVarCount() + 1);
}

void Fourti2IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											 const Term& term,
											 const TermTranslator* translator,
											 bool isFirst,
											 FILE* out) {
  ASSERT(coef != 0);
  ASSERT(translator != 0);
  ASSERT(out != 0);
  ASSERT(term.getVarCount() == translator->getVarCount());

  mpz_out_str(out, 10, coef.get_mpz_t());
  if (term.getVarCount() > 0)
	fputc(' ', out);

  writeTermOfIdeal(term, translator, isFirst, out);  
}

void Fourti2IOHandler::writePolynomialFooter(const VarNames& names,
											 bool wroteAnyGenerators,
											 FILE* out) {
  fputs("(coefficient)", out);

  for (size_t var = 0; var < names.getVarCount(); ++var) {
	fputc(' ', out);
	fputs(names.getName(var).c_str(), out);
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writeIdealHeader(const VarNames& names,
										size_t generatorCount,
										FILE* out) {
  ASSERT(out != 0);

  fprintf(out, "%lu %lu\n",
		  (unsigned long)generatorCount,
		  (unsigned long)names.getVarCount());
}

void Fourti2IOHandler::writeTermOfIdeal(const Term& term,
										const TermTranslator* translator,
										bool isFirst,
										FILE* out) {
  ASSERT(translator != 0);
  ASSERT(out != 0);
  ASSERT(term.getVarCount() == translator->getVarCount());

  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var) {
	if (var != 0)
	  fputc(' ', out);
	const char* exp = translator->getExponentString(var, term[var]);
	if (exp == 0)
	  exp = "0";
	fputs(exp, out);
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writeTermOfIdeal(const vector<mpz_class> term,
										const VarNames& names,
										bool isFirst,
										FILE* out) {
  ASSERT(out != 0);
  ASSERT(term.size() == names.getVarCount());

  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var) {
	if (var != 0)
	  fputc(' ', out);
	mpz_out_str(out, 10, term[var].get_mpz_t());
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writeIdealFooter(const VarNames& names,
										bool wroteAnyGenerators,
										FILE* out) {
  if (names.namesAreDefault())
	return;

  for (size_t var = 0; var < names.getVarCount(); ++var) {
	if (var > 0)
	  fputc(' ', out);
	fputs(names.getName(var).c_str(), out);
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writePolynomialHeader(const VarNames& names,
											 FILE* out) {
  fputs("INTERNAL ERROR: writePolynomialHeader called on object of type\n"
		"Fourti2IOHandler using the overload that does not specify size.\n",
		stderr);
  ASSERT(false);
  exit(1);
}

void Fourti2IOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
  fputs("INTERNAL ERROR: writeIdealHeader called on object of type\n"
		"Fourti2IOHandler using the overload that does not specify size.\n",
		stderr);
  ASSERT(false);
  exit(1);
}

void Fourti2IOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  size_t termCount;
  size_t varCount;

  scanner.readSizeT(termCount);
  scanner.readSizeT(varCount);

  ideal.clearAndSetNames(VarNames(varCount));

  ideal.reserve(termCount);
  for (size_t t = 0; t < termCount; ++t) {
	// Read a term
	ideal.newLastTerm();
	vector<mpz_class>& term = ideal.getLastTermRef();
	for (size_t var = 0; var < varCount; ++var)
	  scanner.readIntegerAndNegativeAsZero(term[var]);
  }

  if (scanner.peekIdentifier()) {
	VarNames names;
	for (size_t var = 0; var < varCount; ++var)
	  names.addVar(scanner.readIdentifier());
	ideal.renameVars(names);
  }
}

void Fourti2IOHandler::readTerm(Scanner& in, const VarNames& names,
								vector<mpz_class>& term) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < names.getVarCount(); ++var)
	in.readIntegerAndNegativeAsZero(term[var]);
}
