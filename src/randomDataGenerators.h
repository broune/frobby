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
#ifndef RANDOM_DATA_GENERATORS_GUARD
#define RANDOM_DATA_GENERATORS_GUARD

#include <vector>

class BigIdeal;

/** Generate an ideal of the form \$fx_1*x_2\f$, \f$x_2*x_3\f$ and so
	on.
*/
void generateLinkedListIdeal(BigIdeal& ideal, size_t variableCount);

/** Generate an ideal where \f$x_(a,b)*x_(c,d)\f$ is a generator when
	\f$(a,b)\f$ and \f$(c,d)\f$ indicate coordinates on a square
	chessboard where the move is a valid king's move.
*/
void generateKingChessIdeal(BigIdeal& ideal, size_t rowsAndColumns);

/** Generate an ideal where \f$x_(a,b)*x_(c,d)\f$ is a generator when
	\f$(a,b)\f$ and \f$(c,d)\f$ indicate coordinates on a square
	chessboard where the move is a valid knight's move.
*/
void generateKnightChessIdeal(BigIdeal& ideal, size_t rowsAndColumns);

/** Generate a random ideal where every edge is a product of two
	different variables. Returns true if the requested number of
	generators was achieved.
*/
bool generateRandomEdgeIdeal
(BigIdeal& ideal, size_t variableCount, size_t generatorCount);

/** Generate a random ideal with exponents in the range [0,
	exponentRange]. Returns true if the requested number of generators
	was achieved.
*/
bool generateRandomIdeal(BigIdeal& bigIdeal,
						 size_t exponentRange,
						 size_t variableCount,
						 size_t generatorCount);

/** Generate a random vector of numbers whose gcd is 1.
 */
void generateRandomFrobeniusInstance(vector<mpz_class>& degrees);

#endif
