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

#ifndef MY_IDEAL_GUARD
#define MY_IDEAL_GUARD

#include <vector>
#include "../src/frobby.h"

// A representation of a power product, i.e. a term without an
// associated coefficient.
typedef std::vector<int> MyPP;

// A representation of a monomial ideal.
typedef std::vector<MyPP> MyIdeal;

// A representation of a set of ideals.
typedef std::vector<MyIdeal> MyIdeals;

MyPP makePP(int e1, int e2, int e3, int e4);
MyIdeals makeIdeals(MyIdeal ideal);

void sortIdeal(MyIdeal& a);

void sortIdeals(MyIdeals& a);

// Returns true if a is equal to b.
bool equal(MyIdeal a, MyIdeal b);
bool equal(MyIdeals a, MyIdeals b);

// Print parameter to standard out.
void printIdeal(MyIdeal ideal);
void printIdeals(MyIdeals ideals);

// Covert from MyIdeal to Frobby::Ideal. Ideal cannot be empty as then
// there is no way to tell how many variables there are, and Frobby
// requires knowing this.
Frobby::Ideal convertToFrobbyIdeal(const MyIdeal& ideal);

#endif
