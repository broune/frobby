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

MyTerm::MyTerm(int coef, MyPP exponentsParam):
  coefficient(coef),
  exponents(exponentsParam) {
}

MyTerm::MyTerm(int coef, int e):
  coefficient(coef), 
  exponents(1) {
  coefficient = coef;
  exponents[0] = e;
}

MyTerm::MyTerm(int coef, int e1, int e2, int e3, int e4):
  coefficient(coef), 
  exponents(4) {
  coefficient = coef;
  exponents[0] = e1;
  exponents[1] = e2;
  exponents[2] = e3;
  exponents[3] = e4;
}

bool MyTerm::operator<(const MyTerm& term) const {
  return exponents < term.exponents ||
	(exponents == term.exponents && coefficient < term.coefficient);
}

bool MyTerm::operator==(const MyTerm& term) const {
  return exponents == term.exponents && coefficient == term.coefficient;
}

bool equal(MyPolynomial a, MyPolynomial b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());

  return a == b;
}

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
