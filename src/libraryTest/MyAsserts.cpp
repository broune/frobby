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

#include "MyAsserts.h"

void assertEqual(const MyIdeal& a, const MyIdeal& b, const char* msg) {
  if (!equal(a, b)) {
	fputs("\nFrobby internal test error: ideals not equal.\n", stdout);
	fputs("Context: ", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);
	printIdeal(a);
	printIdeal(b);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
}

void assertEqual(const MyIdeals& a, const MyIdeals& b, const char* msg) {
  if (!equal(a, b)) {
	fputs("\nFrobby internal test error: ideal sets not equal.\n", stdout);
	fputs("Context: ", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);
	printIdeals(a);
	printIdeals(b);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
}

void assertEqual(const MyPolynomial& a, const MyPolynomial& b,
				 const char* msg) {
  if (!equal(a, b)) {
	fputs("\nFrobby internal test error: Polynomials not equal.\n",
		  stdout);
	fputs("Context: ", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);
	printPolynomial(a);
	printPolynomial(b);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
}

void assertTrue(bool b, const char* msg) {
  if (!b) {
	fputs("\nFrobby internal test error: condition not true.\n", stdout);
	fputs("Context: ", stdout);
	fputs(msg, stdout);
	fputc('\n', stdout);
	exit(1);
  } else {
	fputc('.', stdout);
	fflush(stdout);
  }
}
