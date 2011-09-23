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

/** @file
  This file contains functions that generate data. They are NOT all random.

  @todo Fix the name of the file, since not everything here is random,
  and consider the structure of the code. E.g. does generating
  Frobenius instances belong with the code generating monomial ideals?
  Should these methods be free functions?  Should there be a facade
  which does nearly nothing but to call these functions? Should this
  be the place where the description of these ideals is presented?
  Shouldn't these be returning their output using a consumer?
  Shouldn't there be a clear mapping between these ideals' names as
  strings and the function creating them, rather than just having a
  static list in the Action that creates these?
*/

/** Generate an ideal of the form \f$x_1*x_2\f$, \f$x_2*x_3\f$ and so
    on.
*/
void generateLinkedListIdeal(BigIdeal& ideal, size_t varCount);

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

/** Generate an ideal in n*k variables. Each facet corresponds to a
   way of placing rooks on an n*k chessboard without any rooks being
   able to take any other rook in one move.
*/
void generateRookChessIdeal(BigIdeal& bigIdeal, size_t n, size_t k);

/** Generate an ideal whose facets are the maximum matchings in an
	n-clique. */
void generateMatchingIdeal(BigIdeal& bigIdeal, size_t n);

/** Generate an ideal in varCount variables with minimal generators given by
\f[
\left\langle
  \left.
    \left(
      \prod_{i\in F}x_i
    \right)^{n-|F|+1}
  \right|
  \emptyset\neq F\subseteq
  \left\{
    1,\ldots,n
  \right\}
\right\rangle
,\f]
i.e. to get the minimal generators, take each square free monomial
except the identity, and raise it to the power of varCount + 1 minus
it's total degree.
*/
void generateTreeIdeal(BigIdeal& ideal, size_t varCount);

/** Generate a random ideal where every edge is a product of two
    different variables. Returns true if the requested number of
    generators was achieved.
*/
bool generateRandomEdgeIdeal
(BigIdeal& ideal, size_t varCount, size_t generatorCount);

/** Generate a random ideal with exponents in the range [0,
    exponentRange]. Returns true if the requested number of generators
    was achieved.
*/
bool generateRandomIdeal(BigIdeal& bigIdeal,
                         size_t exponentRange,
                         size_t varCount,
                         size_t generatorCount);


/** Generate a random vector of numbers whose gcd is 1.
 */
void generateRandomFrobeniusInstance(vector<mpz_class>& degrees,
                                     size_t entryCount,
                                     const mpz_class& maxEntry);

#endif
