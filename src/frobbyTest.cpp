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
// This file is an example of how to use the public interface of Frobby.
// It also serves as a test of the public interface of Frobby.

#include "stdinc.h" // Brings in ASSERT.
#include "frobby.h"
#include <vector>
#include <gmp.h>
#include <iostream>
using namespace std;

namespace FrobbyImpl {
  namespace frobbyTest {
    // This is some external representation of a power product.
    typedef vector<int> MyPP;

    // This is some external representation of a monomial ideal.
    typedef vector<MyPP> MyIdeal;

    // These methods create ideals.
    MyIdeal make4_xx_yy_xz_yz();
    MyIdeal make4_xyzzz_xxyy();
    MyIdeal make4_xyz_xxyy();
    MyIdeal make4_x_y();
    MyIdeal make4_xx_yy_z();
    MyIdeal make_1(size_t varCount);
    MyIdeal make_0();

    // ideal cannot be empty since then the number of variables is unspecified.
    Frobby::Ideal convertToFrobbyIdeal(MyIdeal ideal);

    void assertEqual(MyIdeal a, MyIdeal b);

    // This class records the terms it consumes and provides the ideal
    // generated by them.
    class MyTermConsumer : public Frobby::TermConsumer {
    public:
      MyTermConsumer(size_t varCount):
	_varCount(varCount) {
      }

      virtual ~MyTermConsumer() {
      }

      virtual void consume(mpz_ptr* exponentVector) {
	MyPP pp(_varCount);
	for (size_t var = 0; var < _varCount; ++var) {
	  if (!mpz_fits_sint_p(exponentVector[var]))
	    return; // A real program would likely report an error here.
	  pp[var] = mpz_get_si(exponentVector[var]);
	}
	_ideal.push_back(pp);
      }

      MyIdeal getIdeal() const {
	return _ideal;
      }

    private:
      MyIdeal _ideal;
      size_t _varCount;
    };

    void testExplicitPoint();
    void testImplicitPoint();
    void testZeroIdeal();
    void testOneIdeal();

    void testFrobbyInterface() {
      testImplicitPoint();
      testExplicitPoint();
      testZeroIdeal();
      testOneIdeal();
    }

    void testExplicitPoint() {
      const size_t varCount = 4;

      MyIdeal inputIdeal = make4_xx_yy_xz_yz();
      Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);

      MyTermConsumer consumer(varCount);
      mpz_t pointLcm[varCount];
      mpz_init_set_si(pointLcm[0], 2);
      mpz_init_set_si(pointLcm[1], 2);
      mpz_init_set_si(pointLcm[2], 3);
      mpz_init_set_si(pointLcm[3], 100);

      Frobby::alexanderDual(frobbyInputIdeal, pointLcm, consumer);

      mpz_clear(pointLcm[0]);
      mpz_clear(pointLcm[1]);
      mpz_clear(pointLcm[2]);
      mpz_clear(pointLcm[3]);

      assertEqual(consumer.getIdeal(), make4_xyzzz_xxyy());
    }
    
    void testImplicitPoint() {
      size_t varCount = 4;
      MyIdeal inputIdeal = make4_xx_yy_xz_yz();
      Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);
      MyTermConsumer consumer(varCount);

      Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);
      assertEqual(consumer.getIdeal(), make4_xyz_xxyy());
    }

    void testZeroIdeal() {
      // We try the zero ideal with different varCounts.
      for (size_t varCount = 0; varCount <= 3; ++varCount) {
	Frobby::Ideal frobbyInputIdeal(varCount);
	MyTermConsumer consumer(varCount);

	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);
	assertEqual(consumer.getIdeal(), make_0());
      }
    }

    void testOneIdeal() {
      // We try the ideal generated by 1 with different varCounts.
      for (size_t varCount = 0; varCount <= 3; ++varCount) {
	MyIdeal inputIdeal = make_1(varCount);
	Frobby::Ideal frobbyInputIdeal = convertToFrobbyIdeal(inputIdeal);
	MyTermConsumer consumer(varCount);

	Frobby::alexanderDual(frobbyInputIdeal, 0, consumer);
	assertEqual(consumer.getIdeal(), make_1(varCount));
      }
    }

    // Return the ideal in four variables generated by x*x, y*y, x*z and y*z.
    MyIdeal make4_xx_yy_xz_yz() {
      size_t varCount = 4;

      MyPP xx(varCount);
      xx[0] = 2;

      MyPP yy(varCount);
      yy[1] = 2;

      MyPP xz(varCount);
      xz[0] = 1;
      xz[2] = 1;

      MyPP yz(varCount);
      yz[1] = 1;
      yz[2] = 1;

      MyIdeal ideal;
      ideal.push_back(xx);
      ideal.push_back(yy);
      ideal.push_back(xz);
      ideal.push_back(yz);
      return ideal;
    }

    // Return the ideal in four variables generated by x and y.
    MyIdeal make4_x_y() {
      size_t varCount = 4;

      MyPP x(varCount);
      x[0] = 1;

      MyPP y(varCount);
      y[1] = 1;

      MyIdeal ideal;
      ideal.push_back(x);
      ideal.push_back(y);
      return ideal;
    }

    MyIdeal make4_xyzzz_xxyy() {
      size_t varCount = 4;

      MyPP xyzzz(varCount);
      xyzzz[0] = 1;
      xyzzz[1] = 1;
      xyzzz[2] = 3;

      MyPP xxyy(varCount);
      xxyy[0] = 2;
      xxyy[1] = 2;

      MyIdeal ideal;
      ideal.push_back(xyzzz);
      ideal.push_back(xxyy);
      return ideal;
    }

    MyIdeal make4_xyz_xxyy() {
      size_t varCount = 4;

      MyPP xyz(varCount);
      xyz[0] = 1;
      xyz[1] = 1;
      xyz[2] = 1;

      MyPP xxyy(varCount);
      xxyy[0] = 2;
      xxyy[1] = 2;

      MyIdeal ideal;
      ideal.push_back(xyz);
      ideal.push_back(xxyy);
      return ideal;
    }

    // Return the ideal in 4 variables generated by x*x, y*y and z.
    MyIdeal make4_xx_yy_z() {
      size_t varCount = 4;

      MyPP xx(varCount);
      xx[0] = 2;

      MyPP yy(varCount);
      yy[1] = 2;

      MyPP z(varCount);
      z[2] = 1;

      MyIdeal ideal;
      ideal.push_back(xx);
      ideal.push_back(yy);
      ideal.push_back(z);

      return ideal;
    }

    // Return the ideal in varCount variables generated by 1.
    MyIdeal make_1(size_t varCount) {
      MyPP one(varCount);
      MyIdeal ideal;
      ideal.push_back(one);
      return ideal;
    }

    // Return the zero ideal in some unspecified number of variables.
    MyIdeal make_0() {
      return MyIdeal();
    }

    void printIdeal(MyIdeal ideal) {
      fputs("Ideal:\n", stderr);
      for (size_t gen = 0; gen < ideal.size(); ++gen) {
	for (size_t var = 0; var < ideal[gen].size(); ++var)
	  fprintf(stderr, " %i", ideal[gen][var]);
	fputc('\n', stderr);
      }
    }

    void assertEqual(MyIdeal a, MyIdeal b) {
      sort(a.begin(), a.end());
      sort(b.begin(), b.end());
      if (a != b) {
	fputs("Assert error: ideals not equal.\n", stderr);
	printIdeal(a);
	printIdeal(b);
      } else
	fputs("test passed\n", stderr);
    }

    Frobby::Ideal convertToFrobbyIdeal(MyIdeal ideal) {
      ASSERT(!ideal.empty());
      size_t varCount = ideal[0].size();

      Frobby::Ideal frobbyIdeal(varCount);
      for (size_t generator = 0; generator < ideal.size(); ++generator) {
	for (size_t var = 0; var < varCount || var == 0; ++var)
	  frobbyIdeal.addExponent(ideal[generator][var]);
      }

      return frobbyIdeal;
    }
  }
}
