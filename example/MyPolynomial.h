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

// Represents a term with a coefficient.
struct MyTerm {
  MyTerm(int coef, MyPP exponents):
	coefficient(coef),
	   exponents(exponents) {
  }

  MyTerm(int coef, int e):
	coefficient(coef), 
	   exponents(1) {
	coefficient = coef;
	exponents[0] = e;
  }

  MyTerm(int coef, int e1, int e2, int e3, int e4):
	coefficient(coef), 
	   exponents(4) {
	coefficient = coef;
	exponents[0] = e1;
	exponents[1] = e2;
	exponents[2] = e3;
	exponents[3] = e4;
  }

  int coefficient;
  MyPP exponents;

  // For use by std::sort.
  bool operator<(const MyTerm& term) const {
	return exponents < term.exponents ||
	  (exponents == term.exponents && coefficient < term.coefficient);
  }

  bool operator==(const MyTerm& term) const {
	return exponents == term.exponents && coefficient == term.coefficient;
  }
};

// Represents a polynomial.
typedef vector<MyTerm> MyPolynomial;


// Returns true if a is equal to b.
bool equal(MyPolynomial a, MyPolynomial b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());

  return a == b;
}

// Print ideal to standard out.
void printPolynomial(MyPolynomial polynomial) {
  std::sort(polynomial.begin(), polynomial.end());

  fprintf(stdout, "Polynomial (%lu terms):\n",
		  (unsigned long)polynomial.size());
  for (size_t term = 0; term < polynomial.size(); ++term) {
	fprintf(stdout, " %i ", polynomial[term].coefficient);
	for (size_t var = 0; var < polynomial[term].exponents.size(); ++var)
	  fprintf(stdout, " %i", polynomial[term].exponents[var]);
	fputc('\n', stdout);
  }
}

// Print an error and exit if a is not equal to b. Otherwise print a
// dot to signify that a test passed.
void assertEqual(const MyPolynomial& a, const MyPolynomial& b) {
  if (!equal(a, b)) {
	fputs("\nFrobby library interface test error: Polynomials not equal.\n",
		  stdout);
	printPolynomial(a);
	printPolynomial(b);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
}
