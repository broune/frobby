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

class BigIdeal;

class IdealFacade : private Facade {
 public:
  IdealFacade(bool printActions);

  // Applies some generic deformation to the ideal.
  void deform(BigIdeal& ideal);

  // Takes the radical of the ideal and minimizes it.
  void takeRadical(BigIdeal& ideal);

  // Removes redundant generators from ideal.
  void sortAllAndMinimize(BigIdeal& bigIdeal);

  // Clears the input ideal and writes to file.
  void sortAllAndMinimize(BigIdeal& bigIdeal, FILE* out, const string& format);

  // Sorts the generators of ideal and removes duplicates.
  void sortGeneratorsUnique(BigIdeal& ideal);

  // Sorts the generators of ideal.
  void sortGenerators(BigIdeal& ideal);

  // Sorts the variables of ideal.
  void sortVariables(BigIdeal& ideal);


  void printAnalysis(FILE* out, BigIdeal& ideal);
  void printLcm(FILE* out, BigIdeal& ideal);
};

#endif
