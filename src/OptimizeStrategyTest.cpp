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
#include "stdinc.h"
#include "OptimizeStrategy.h"
#include "tests.h"

#include "IdealFactory.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "TermGrader.h"
#include "SplitStrategy.h"
#include "Slice.h"
#include "SliceAlgorithm.h"

#include <vector>

TEST_SUITE(SliceStrategy)
TEST_SUITE2(SliceStrategy, OptimizeStrategy)

namespace {
  vector<mpz_class> makeVector(mpz_class a, mpz_class b, mpz_class c, mpz_class d) {
	vector<mpz_class> vec(4);
	vec[0] = a;
	vec[1] = b;
	vec[2] = c;
	vec[3] = d;
	return vec;
  }
}

TEST(OptimizeStrategy, Simplify) {
  Ideal ideal;
  TermTranslator translator(IdealFactory::xx_yy_zz_t_xz_yz(), ideal, false);
  TermGrader grader(makeVector(0, 100, 10000, mpz_class("300000000000000007")), &translator);
  auto_ptr<SplitStrategy> splitStrategy = SplitStrategy::createStrategy("median");
  OptimizeStrategy strategy(grader, splitStrategy.get(), false, true);

  runSliceAlgorithm(ideal, strategy);

  ASSERT_EQ(strategy.getMaximalSolutions(), Ideal(Term("1 1 2 1")));
  ASSERT_EQ(strategy.getMaximalValue(), mpz_class("300000000000020107"));
}
