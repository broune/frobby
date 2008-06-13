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

// A representation of a power product, i.e. a term without an
// associated coefficient.
typedef std::vector<int> MyPP;

// A representation of a monomial ideal.
typedef std::vector<MyPP> MyIdeal;

// A representation of a set of ideals.
typedef std::vector<MyIdeal> MyIdeals;

MyPP makePP(int e1, int e2, int e3, int e4) {
  MyPP pp(4);
  pp[0] = e1;
  pp[1] = e2;
  pp[2] = e3;
  pp[3] = e4;
  return pp;
}

MyIdeals makeIdeals(MyIdeal ideal) {
  MyIdeals ideals;
  ideals.push_back(ideal);
  return ideals;
}

void sortIdeal(MyIdeal& a) {
  std::sort(a.begin(), a.end());
}

void sortIdeals(MyIdeals& a) {
  for (size_t i = 0; i < a.size(); ++i)
	sortIdeal(a[i]);
}

// Returns true if a is equal to b.
bool equal(MyIdeal a, MyIdeal b) {
  sortIdeal(a);
  sortIdeal(b);
  return a == b;
}

bool equal(MyIdeals a, MyIdeals b) {
  sortIdeals(a);
  sortIdeals(b);
  return a == b;
}

// Print ideal to standard out.
void printIdeal(MyIdeal ideal) {
  fprintf(stdout, "Ideal (%lu generator(s)):\n",
		  (unsigned long)ideal.size());
  for (size_t gen = 0; gen < ideal.size(); ++gen) {
	for (size_t var = 0; var < ideal[gen].size(); ++var)
	  fprintf(stdout, " %i", ideal[gen][var]);
	fputc('\n', stdout);
  }
}

void printIdeals(MyIdeals ideals) {
  fprintf(stdout, "*** Set of %lu ideals:\n",
		  (unsigned long)ideals.size());
  for (size_t i = 0; i < ideals.size(); ++i)
	printIdeal(ideals[i]);
}

// Covert from MyIdeal to Frobby::Ideal. Ideal cannot be empty as then
// there is no way to tell how many variables there are, and Frobby
// requires knowing this.
Frobby::Ideal convertToFrobbyIdeal(const MyIdeal& ideal) {
  size_t varCount = ideal[0].size();

  Frobby::Ideal frobbyIdeal(varCount);
  for (size_t generator = 0; generator < ideal.size(); ++generator) {
	for (size_t var = 0; var < varCount || var == 0; ++var)
	  frobbyIdeal.addExponent(ideal[generator][var]);
  }

  return frobbyIdeal;
}
