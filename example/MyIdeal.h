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



// Returns true if a is equal to b.
bool equal(MyIdeal a, MyIdeal b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());

  return a == b;
}

// Print ideal to standard out.
void printIdeal(MyIdeal ideal) {
  fputs("Ideal:\n", stdout);
  for (size_t gen = 0; gen < ideal.size(); ++gen) {
	for (size_t var = 0; var < ideal[gen].size(); ++var)
	  fprintf(stdout, " %i", ideal[gen][var]);
	fputc('\n', stdout);
  }
}

// Print an error and exit if a is not equal to b. Otherwise print a
// dot to signify that a test passed.
void assertEqual(const MyIdeal& a, const MyIdeal& b) {
  if (!equal(a, b)) {
	fputs("\nError: ideals not equal.\n", stdout);
	printIdeal(a);
	printIdeal(b);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
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
