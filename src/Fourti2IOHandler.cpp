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
#include "BigPolynomial.h"
#include "error.h"
#include "BigTermConsumer.h"
#include "DataType.h"
#include "FrobbyStringStream.h"
#include "IdealConsolidator.h"
#include "PolynomialConsolidator.h"

Fourti2IOHandler::Fourti2IOHandler():
  IOHandler(staticGetName(),
			"Format used by the software package 4ti2.", true) {
  registerInput(DataType::getMonomialIdealType());
  registerInput(DataType::getMonomialIdealListType());
  registerInput(DataType::getPolynomialType());
  registerOutput(DataType::getMonomialIdealType());
  registerOutput(DataType::getMonomialIdealListType());
  registerOutput(DataType::getPolynomialType());
}

const char* Fourti2IOHandler::staticGetName() {
  return "4ti2";
}

void Fourti2IOHandler::writeRing(const VarNames& names, FILE* out) {
  fputs("42 ring\n", out);
  writeRingWithoutHeader(names, out);
}

void Fourti2IOHandler::writeRingWithoutHeader(const VarNames& names,
											  FILE* out) {
  if (names.getVarCount() == 0)
	return;

  fputc(' ', out);
  for (size_t var = 0; var < names.getVarCount(); ++var) {
	if (var > 0)
	  fputc(' ', out);
	fputs(names.getName(var).c_str(), out);
  }
  fputc('\n', out);  
}

void Fourti2IOHandler::writeTerm(const vector<mpz_class>& term,
								 const VarNames& names,
								 FILE* out) {
  writeTermOfIdeal(term, names, false, out);
}

auto_ptr<BigTermConsumer> Fourti2IOHandler::createIdealWriter(FILE* out) {
  ASSERT(supportsOutput(DataType::getMonomialIdealType()));

  FrobbyStringStream msg;
  msg << "Using the format " << getName() <<
	" makes it necessary to store all of the output in "
	"memory before writing it out. This increases "
	"memory consumption and decreases performance.";
  displayNote(msg);

  auto_ptr<BigTermConsumer> writer(IOHandler::createIdealWriter(out));
  auto_ptr<BigTermConsumer> consolidated(new IdealConsolidator(writer));
  return consolidated;
}

auto_ptr<CoefBigTermConsumer> Fourti2IOHandler::createPolynomialWriter
(FILE* out) {
  FrobbyStringStream msg;
  msg << "Using the format " << getName() <<
	" makes it necessary to store all of the output in "
	"memory before writing it out. This increases "
	"memory consumption and decreases performance.";
  displayNote(msg);

  auto_ptr<CoefBigTermConsumer> writer(IOHandler::createPolynomialWriter(out));
  auto_ptr<CoefBigTermConsumer> consolidated
	(new PolynomialConsolidator(writer));
  return consolidated;
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
  ASSERT(translator != 0);
  ASSERT(out != 0);
  ASSERT(term.getVarCount() == translator->getVarCount());

  mpz_out_str(out, 10, coef.get_mpz_t());

  if (term.getVarCount() > 0) {
	fputc(' ', out);
	writeTermOfIdeal(term, translator, isFirst, out);  
  } else
	fputc('\n', out);
}

void Fourti2IOHandler::writeTermOfPolynomial(const mpz_class& coef,
											 const vector<mpz_class>& term,
											 const VarNames& names,
											 bool isFirst,
											 FILE* out) {
  ASSERT(out != 0);
  ASSERT(term.size() == names.getVarCount());

  mpz_out_str(out, 10, coef.get_mpz_t());

  if (!term.empty()) {
	fputc(' ', out);
	writeTermOfIdeal(term, names, isFirst, out);  
  } else
	fputc('\n', out);
}

void Fourti2IOHandler::writePolynomialFooter(const VarNames& names,
											 bool wroteAnyGenerators,
											 FILE* out) {
  fputs("(coefficient)", out);
  if (!names.namesAreDefault())
	writeRingWithoutHeader(names, out);
  else
	fputc('\n', out);
}

void Fourti2IOHandler::writeIdealHeader(const VarNames& names,
										bool defineNewRing,
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
  if (varCount == 0)
	return; // Avoids the newline.

  for (size_t var = 0; var < varCount; ++var) {
	fputc(' ', out);
	const char* exp = translator->getExponentString(var, term[var]);
	if (exp == 0)
	  exp = "0";
	fputs(exp, out);
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writeTermOfIdeal(const vector<mpz_class>& term,
										const VarNames& names,
										bool isFirst,
										FILE* out) {
  ASSERT(out != 0);
  ASSERT(term.size() == names.getVarCount());

  size_t varCount = term.size();
  if (varCount == 0)
	return; // Avoids the newline.

  for (size_t var = 0; var < varCount; ++var) {
	fputc(' ', out);
	mpz_out_str(out, 10, term[var].get_mpz_t());
  }
  fputc('\n', out);
}

void Fourti2IOHandler::writeIdealFooter(const VarNames& names,
										bool wroteAnyGenerators,
										FILE* out) {
  if (!names.namesAreDefault())
	writeRingWithoutHeader(names, out);
}

void Fourti2IOHandler::writePolynomialHeader(const VarNames& names,
											 FILE* out) {
  ASSERT(false);
  reportInternalError
	("The method writePolynomialHeader called on object of type "
	 "Fourti2IOHandler using the overload that does not specify size.");
}

void Fourti2IOHandler::writeIdealHeader(const VarNames& names,
										bool defineNewRing,
										FILE* out) {
  ASSERT(false);
  reportInternalError
	("The method writeIdealHeader called on object of type "
	 "Fourti2IOHandler using the overload that does not specify size.");
}

// The parsing of the header with the number of generators and
// variables has to be separate from the code below reading the rest
// of the ideal, since in some contexts what looks like the header of
// an ideal could be a ring description, while in other contexts this
// cannot happen.
void Fourti2IOHandler::readIdeal(Scanner& in, BigTermConsumer& consumer,
								 size_t generatorCount, size_t varCount) {
  // We have to read the entire ideal before we can tell whether there is
  // a ring associated to it, so we have to store the ideal here until
  // that time.

  BigIdeal ideal((VarNames(varCount)));
  ideal.reserve(generatorCount);
  for (size_t t = 0; t < generatorCount; ++t) {
	// Read a term
	ideal.newLastTerm();
	vector<mpz_class>& term = ideal.getLastTermRef();
	for (size_t var = 0; var < varCount; ++var)
	  in.readIntegerAndNegativeAsZero(term[var]);
  }

  if (in.peekIdentifier()) {
	VarNames names;
	readRing(in, names, varCount);
	ideal.renameVars(names);
  }

  consumer.consume(ideal);
}

void Fourti2IOHandler::readIdeal(Scanner& in, BigTermConsumer& consumer) {
  size_t generatorCount;
  in.readSizeT(generatorCount);

  size_t varCount;
  in.readSizeT(varCount);

  readIdeal(in, consumer, generatorCount, varCount);
}

void Fourti2IOHandler::readIdeals(Scanner& in, BigTermConsumer& consumer) {
  // An empty list is just a ring by itself, and this has a special
  // syntax.  So we first decipher whether we are looking at a ring or
  // an ideal.  At the point where we can tell that it is an ideal, we
  // have already read part of the ideal, so we have to do something
  // to pass the read information on to the code for reading the rest
  // of the ideal.

  size_t generatorCount;
  in.readSizeT(generatorCount);

  if (generatorCount == 42 && in.peekIdentifier()) {
	in.expect("ring");
	VarNames names;
	readRing(in, names);
	consumer.consumeRing(names);
	in.expectEOF();
	return;
  }

  size_t varCount;
  in.readSizeT(varCount);

  readIdeal(in, consumer, generatorCount, varCount);

  while (hasMoreInput(in))
	readIdeal(in, consumer);
}

void Fourti2IOHandler::readRing(Scanner& in, VarNames& names) {
  names.clear();
  while (in.peekIdentifier())
	names.addVarSyntaxCheckUnique(in, in.readIdentifier());
}

void Fourti2IOHandler::readRing(Scanner& in,
								VarNames& names,
								size_t varCount) {
  names.clear();
  for (size_t var = 0; var < varCount; ++var)
	names.addVarSyntaxCheckUnique(in, in.readIdentifier());
}

void Fourti2IOHandler::readTerm(Scanner& in, const VarNames& names,
								vector<mpz_class>& term) {
  term.resize(names.getVarCount());
  for (size_t var = 0; var < names.getVarCount(); ++var)
	in.readIntegerAndNegativeAsZero(term[var]);
}

void Fourti2IOHandler::readPolynomial
(Scanner& in, CoefBigTermConsumer& consumer) {
  size_t generatorCount;
  size_t varCount;

  in.readSizeT(generatorCount);
  in.readSizeT(varCount);

  if (varCount == 0)
	reportError
	  ("A polynomial has at least one column in the matrix,"
	   "but this matrix has no columns.");

  --varCount; // One of columns is the coefficient.

  BigPolynomial polynomial((VarNames(varCount)));

  for (size_t t = 0; t < generatorCount; ++t) {
	// Read a term
	polynomial.newLastTerm();
	in.readInteger(polynomial.getLastCoef());

	vector<mpz_class>& term = polynomial.getLastTerm();
	for (size_t var = 0; var < varCount; ++var) {
	  ASSERT(var < term.size());
	  in.readIntegerAndNegativeAsZero(term[var]);
	}
  }

  if (!in.match('('))
	in.expect("(coefficient)"); // This improves the error message.
  in.expect("coefficient");
  in.expect(')');

  if (in.peekIdentifier()) {
	VarNames names;
	for (size_t var = 0; var < varCount; ++var)
	  names.addVar(in.readIdentifier());
	polynomial.renameVars(names);
  }  

  consumer.consume(polynomial);
}
