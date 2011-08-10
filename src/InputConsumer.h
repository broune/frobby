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
#ifndef INPUT_CONSUMER_GUARD
#define INPUT_CONSUMER_GUARD

#include "BigIdeal.h"
#include "VarNames.h"
#include "ElementDeleter.h"
#include <vector>
#include <list>

class Scanner;

class InputConsumer {
 public:
  InputConsumer();

  void consumeRing(const VarNames& names);
  void consumeIdeal(auto_ptr<BigIdeal> ideal);
  
  /** Start consuming an ideal. */
  void beginIdeal();

  /** Suggest that the current ideal will have the given number of
	  generators. This helps in preallocating the right amount of
	  memory. */
  void hintGenCount(size_t hintGenCount);

  /** Start consuming a term. */
  void beginTerm();

  /** Reads variable as a number so that the first variable is 1.
   Does not return if there is an error. */ 
  size_t consumeVarNumber(Scanner& in);

  /** Consumes var raised to the exponent 1. */
  void consumeVarExponentOne(size_t var, const Scanner& in);

  /** Consumes var raised to an exponent read from in.

   Does not return if there is an error. */
  void consumeVarExponent(size_t var, Scanner& in);

  /** Consumes var raised to an exponent read from in. If the number is
   negative then that is read as zero.

   Does not return if there is an error. */
  void consumeVarExponentNegativeAsZero(size_t var, Scanner& in);

  /** Done reading a term. */
  void endTerm() {}

  /** Consume a term in one go. Term is interpreted as an exponent
	  vector. */
  void consumeTerm(const vector<mpz_class>& term);

  /** Reads a term in a format like "a^4*b*c^2" */
  void consumeTermProductNotation(Scanner& in);

  /** Done reading an ideal. */
  void endIdeal();

  /** Returns true if there are ideals stored. */
  bool empty() const {return _ideals.empty();}

  /** Returns the */
  auto_ptr<BigIdeal> releaseIdeal();
  const VarNames& getRing() const {return _names;}


 private:
  VarNames _names;
  std::list<BigIdeal*> _ideals;
  ElementDeleter<std::list<BigIdeal*> > _idealsDeleter;
  bool _inIdeal;
};

#endif
