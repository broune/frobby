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
#ifndef IDEAL_FACADE_GUARD
#define IDEAL_FACADE_GUARD

#include "Facade.h"

#include <vector>

class BigIdeal;
class IOHandler;

class IdealFacade : private Facade {
 public:
  IdealFacade(bool printActions);

  // Applies some generic deformation to the ideal.
  void deform(BigIdeal& ideal);

  // Takes the radical of the generators of ideal. Non-minimal
  // generators that may appear due to this are not removed.
  void takeRadical(BigIdeal& ideal);

  // Take the product of the minimal generators of each ideal, and add
  // the resulting monomials as generators of ideal. Requires that
  // each ideal have the same names, including ideal.
  void takeProducts(const vector<BigIdeal*>& ideals, BigIdeal& ideal);

  // Removes redundant generators from ideal.
  void sortAllAndMinimize(BigIdeal& bigIdeal);

  // Adds x_i^(l_i+1) to the ideal for each i where that will be a
  // minimal generator, where x^l is the lcm of the generators of
  // bigIdeal.
  void addPurePowers(BigIdeal& bigIdeal);

  // Sorts the generators of ideal and removes duplicates.
  void sortGeneratorsUnique(BigIdeal& ideal);

  // Sorts the generators of ideal.
  void sortGenerators(BigIdeal& ideal);

  // Sorts the variables of ideal.
  void sortVariables(BigIdeal& ideal);

  void printAnalysis(FILE* out, BigIdeal& ideal);
  void printLcm(BigIdeal& ideal,
				IOHandler* handler,
				FILE* out);
};

#endif
