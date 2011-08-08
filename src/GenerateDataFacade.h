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
#ifndef GENERATE_DATA_FACADE_GUARD
#define GENERATE_DATA_FACADE_GUARD

#include "Facade.h"
#include <vector>

class BigIdeal;

/** A facade for random and other kinds of data.

    @ingroup Facade
*/
class GenerateDataFacade : private Facade {
 public:
  GenerateDataFacade(bool printActions);

  void generateListIdeal(BigIdeal& ideal, size_t varCount);
  void generateKingChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);
  void generateKnightChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);
  void generateRookChessIdeal(BigIdeal& ideal, unsigned int n, unsigned int k);
  void generateMatchingIdeal(BigIdeal& ideal, unsigned int n);

  void generateTreeIdeal(BigIdeal& ideal, unsigned int varCount);

  void generateEdgeIdeal
    (BigIdeal& ideal, size_t varCount, size_t generatorCount);

  void generateIdeal(BigIdeal& ideal,
                     size_t exponentRange,
                     size_t varCount,
                     size_t generatorCount);

  void generateFrobeniusInstance(vector<mpz_class>& instance,
                                 size_t entryCount,
                                 const mpz_class& maxEntry);
};

#endif
