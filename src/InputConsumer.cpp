/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#include "InputConsumer.h"

#include "Scanner.h"
#include "FrobbyStringStream.h"
#include "error.h"

#include <iostream>
namespace {
  void readVarPower(vector<mpz_class>& term,
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
}

InputConsumer::InputConsumer(): _idealsDeleter(_ideals) {
}

void InputConsumer::consumeRing(const VarNames& names) {
  _names = names;
}

void InputConsumer::consumeIdeal(auto_ptr<BigIdeal> ideal) {
  _names = ideal->getNames();
  exceptionSafePushBack(_ideals, ideal);
}

void InputConsumer::beginIdeal() {
  auto_ptr<BigIdeal> ideal(new BigIdeal(_names));
  exceptionSafePushBack(_ideals, ideal);
}

void InputConsumer::beginTerm() {
  ASSERT(!empty());
  _ideals.back()->newLastTerm();
}

size_t InputConsumer::consumeVarNumber(Scanner& in) {
  ASSERT(!empty());
  ASSERT(!_ideals.back()->getGeneratorCount() == 0);

  size_t var;
  in.readSizeT(var);

  if (var == 0 || var > _ideals.back()->getVarCount()) {
	FrobbyStringStream errorMsg;
	errorMsg << "There is no variable number " << var << '.';
	reportSyntaxError(in, errorMsg);
  }
  return var - 1;
}

void InputConsumer::consumeVarExponentOne(size_t var, const Scanner& in) {
  ASSERT(!empty());
  ASSERT(var < _ideals.back()->getVarCount());
  mpz_class& exponent = _ideals.back()->getLastTermExponentRef(var);
  if (var != 0) {
	FrobbyStringStream errorMsg;
	errorMsg << "The variable " << _names.getName(var)
			 << " appears twice in the same monomial.";
	reportSyntaxError(in, errorMsg);
  }
  exponent = 1;
}

void InputConsumer::consumeVarExponent(size_t var, Scanner& in) {
  ASSERT(!empty());
  ASSERT(var < _ideals.back()->getVarCount());

  mpz_class& exponent = _ideals.back()->getLastTermExponentRef(var);
  in.readInteger(exponent);
  if (exponent <= 0) {
	FrobbyStringStream errorMsg;
	errorMsg << "Expected positive integer as exponent but got "
			 << exponent << '.';
	reportSyntaxError(in, errorMsg);
  }
}

void InputConsumer::consumeTerm(const vector<mpz_class>& term) {
  ASSERT(!empty());
  ASSERT(_ideals.back()->getVarCount() == term.size());
  _ideals.back()->insert(term);
}

void InputConsumer::consumeTermProductNotation(Scanner& in) {
  ASSERT(!empty());
  _ideals.back()->newLastTerm();
  vector<mpz_class>& term = _ideals.back()->getLastTermRef();

  if (in.match('1'))
    return;
  do {
    readVarPower(term, _names, in);
  } while (in.match('*'));
}

auto_ptr<BigIdeal> InputConsumer::releaseIdeal() {
  ASSERT(!empty());
  auto_ptr<BigIdeal> ideal(_ideals.front());
  _ideals.pop_front();
  return ideal;
}
