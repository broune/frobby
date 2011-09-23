/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "Matrix.h"
#include "tests.h"

#include "BigIntVector.h"

TEST_SUITE(Matrix)

namespace {
  Matrix makeMatrix(size_t colCount, const char* entries) {
	Matrix mat;
	istringstream in(entries);
	mpq_class entry;
	while (in >> entry) {
	  mat.resize(mat.getRowCount() + 1, colCount);
	  mat(mat.getRowCount() - 1, 0) = entry;
	  for (size_t col = 1; col < colCount; ++col)
		in >> mat(mat.getRowCount() - 1, col);
	}
	return mat;
  }

  Matrix make123() {
	return makeMatrix(4,
					  "1 2 3 1\n"
					  "4 5 6 1\n"
					  "7 8 9 1\n");
  }

  BigIntVector makeVector(const char* entries) {
	BigIntVector vec(0);
	istringstream in(entries);
	mpz_class entry;
	while (in >> entry) {
	  vec.resize(vec.getSize() + 1);
	  vec[vec.getSize() - 1] = entry;
	}
	return vec;
  }
}

TEST(Matrix, Basic) {
  Matrix mat = make123();
  ASSERT_EQ(mat.getRowCount(), 3u);
  ASSERT_EQ(mat.getColCount(), 4u);
  ASSERT_EQ(mat(0, 0), 1);
  ASSERT_EQ(mat(2, 3), 1);
  ASSERT_EQ(mat(0, 1), 2);

  const Matrix con = mat;
  ASSERT_EQ(con.getRowCount(), 3u);
  ASSERT_EQ(con.getColCount(), 4u);
  ASSERT_EQ(con(0, 0), 1);
  ASSERT_EQ(con(2, 3), 1);
  ASSERT_EQ(con(0, 1), 2);
}

TEST(Matrix, Resize) {
  Matrix mat = make123();

  mat.resize(10, 10);
  ASSERT_EQ(mat.getRowCount(), 10u);
  ASSERT_EQ(mat.getColCount(), 10u);
  ASSERT_EQ(mat(0, 1), 2);
  ASSERT_EQ(mat(2, 3), 1);
  ASSERT_EQ(mat(9, 9), 0);

  mat.resize(1, 2);
  ASSERT_EQ(mat.getRowCount(), 1u);
  ASSERT_EQ(mat.getColCount(), 2u);
  ASSERT_EQ(mat(0, 1), 2);

  mat.resize(0, 0);
  ASSERT_EQ(mat.getRowCount(), 0u);
  ASSERT_EQ(mat.getColCount(), 0u);

  mat.resize(2, 1);
  ASSERT_EQ(mat.getRowCount(), 2u);
  ASSERT_EQ(mat.getColCount(), 1u);
  ASSERT_EQ(mat(1, 0), 0);
}

TEST(Matrix, Swap) {
  Matrix mat1 = make123();
  Matrix mat2(2, 1);
  mat2(1, 0) = 42;
  Matrix mat2Copy = mat2;

  swap(mat1, mat2);

  ASSERT_EQ(mat2, make123());
  ASSERT_EQ(mat1, mat2Copy);
}

TEST(Matrix, Transpose) {
  Matrix mat = make123();
  Matrix trans;
  transpose(trans, mat);
  ASSERT_EQ(trans.getRowCount(), 4u);
  ASSERT_EQ(trans.getColCount(), 3u);
  ASSERT_EQ(trans(3, 2), 1);
  ASSERT_EQ(trans(2, 2), 9);
}

TEST(Matrix, AddMultiplyRow) {
  Matrix mat = make123();

  addMultiplyRow(mat, 2, 0, -3);
  ASSERT_EQ(mat, makeMatrix(4,
							"1 2 3 1\n"
							"4 5 6 1\n"
							"4 2 0 -2\n"));

  addMultiplyRow(mat, 0, 2, mpq_class("-3/2"));
  ASSERT_EQ(mat, makeMatrix(4,
							"-5 -1 3 4\n"
							"4 5 6 1\n"
							"4 2 0 -2\n"));

  addMultiplyRow(mat, 2, 2, 2);
  ASSERT_EQ(mat, makeMatrix(4,
							"-5 -1 3 4\n"
							"4 5 6 1\n"
							"12 6 0 -6\n"));
}

TEST(Matrix, MultiplyRows) {
  Matrix mat = make123();
  multiplyRow(mat, 1, 2);
  ASSERT_EQ(mat, makeMatrix(4,
							"1 2 3 1\n"
							"8 10 12 2\n"
							"7 8 9 1\n"));
}

TEST(Matrix, SwapRows) {
  Matrix mat = make123();

  swapRows(mat, 1, 1);
  ASSERT_EQ(mat, make123());

  swapRows(mat, 0, 2);
  ASSERT_EQ(mat, makeMatrix(4,
							"7 8 9 1\n"
							"4 5 6 1\n"
							"1 2 3 1\n"));
}

TEST(Matrix, RowReduceAndFully1) {
  Matrix mat = make123();
  rowReduce(mat);
  ASSERT_EQ(mat, makeMatrix(4,
							"1 2 3 1\n"
							"0 -3 -6 -3\n"
							"0 0 0 0\n"));

  rowReduceFully(mat);
  ASSERT_EQ(mat, makeMatrix(4,
							"1 0 -1 -1\n"
							"0 1 2 1\n"
							"0 0 0 0\n"));
}

TEST(Matrix, RowReduceAndFully2) {
  Matrix mat = makeMatrix(5,
						  "-4 -8  3   0 23\n"
						  "-4 -8  2  -1  8\n"
						  " 2  4 -1 3/2  4\n");
  Matrix red = makeMatrix(5,
						  "1 2 0  0 -1/2\n"
						  "0 0 1  0    7\n"
						  "0 0 0  1    8\n");

  rowReduceFully(mat);
  ASSERT_EQ(mat, red);

  rowReduce(mat);
  ASSERT_EQ(mat, red);

  rowReduceFully(mat);
  ASSERT_EQ(mat, red);
}

TEST(Matrix, RowReduceAndFully3) {
  Matrix mat(0, 0);

  rowReduce(mat);
  ASSERT_EQ(mat, Matrix(0, 0));

  rowReduceFully(mat);
  ASSERT_EQ(mat, Matrix(0, 0));
}

TEST(Matrix, RowReduceAndFully4) {
  Matrix mat(3, 7);

  rowReduce(mat);
  ASSERT_EQ(mat, Matrix(3, 7));

  rowReduceFully(mat);
  ASSERT_EQ(mat, Matrix(3, 7));
}

TEST(Matrix, SubMatrix) {
  Matrix mat = make123();
  Matrix sub;

  subMatrix(sub, mat, 0, 3, 0, 4);
  ASSERT_EQ(sub, mat);

  subMatrix(sub, mat, 1, 1, 0, 0);
  ASSERT_EQ(sub, Matrix(0, 0));

  subMatrix(sub, mat, 0, 2, 2, 3);
  ASSERT_EQ(sub, makeMatrix(1, "3\n6\n"));

  subMatrix(mat, mat, 1, 2, 2, 3);
  ASSERT_EQ(mat, makeMatrix(1, "6\n"));
}

TEST(Matrix, Inverse) {
  Matrix mat = makeMatrix(3,
						  "1 3 3\n"
						  "1 4 3\n"
						  "1 3 4\n");
  Matrix inv;

  inverse(inv, mat);
  ASSERT_EQ(inv, makeMatrix(3,
							" 7 -3 -3\n"
							"-1  1  0\n"
							"-1  0  1\n"));

  inverse(mat, mat);
  ASSERT_EQ(inv, mat);
}

TEST(Matrix, NullSpace1) {
  Matrix mat = makeMatrix(6,
						  "1 2 0 0 3 0\n"
						  "0 0 1 0 7 0\n"
						  "0 0 0 1 8 0\n"
						  "0 0 0 0 0 0\n");
  Matrix basis;

  nullSpace(basis, mat);
  ASSERT_EQ(basis, makeMatrix(3,
							  "-2 -3 0\n"
							  " 1  0 0\n"
							  " 0 -7 0\n"
							  " 0 -8 0\n"
							  " 0  1 0\n"
							  " 0  0 1\n"));
}

TEST(Matrix, NullSpace2) {
  Matrix mat = make123();
  Matrix basis;

  nullSpace(basis, mat);
  ASSERT_EQ(basis, makeMatrix(2,
							  " 1  1\n"
							  "-2 -1\n"
							  " 1  0\n"
							  " 0  1\n"));
}

TEST(Matrix, NullSpace3) {
  Matrix mat = makeMatrix(6,
						  "13 -171   29   41   37   17\n"
						  "11   61 -278  131   19   23\n"
						  "-3   53  123 -317   41    7\n"
						  "11   97   23   47 -297   41\n"
						  "13   19   11   37   61 -143\n");
  Matrix basis;

  nullSpace(basis, mat);
  ASSERT_EQ(basis, makeMatrix(1,
							  "36441469497\n"
							  " 6795794100\n"
							  " 5893319208\n"
							  " 4007175176\n"
							  " 5788247217\n"
							  " 8175064786\n"));
}

TEST(Matrix, Solve1) {
  Matrix lhs = make123();
  Matrix rhs = makeMatrix(2,
						  "5 5\n"
						  "14 13\n"
						  "26 23\n");
  bool hasSolution = solve(lhs, lhs, rhs);

  ASSERT_FALSE(hasSolution);
  ASSERT_EQ(lhs, make123()); // no change if no solution
}

TEST(Matrix, Solve2) {
  Matrix lhs = make123();
  Matrix rhs = makeMatrix(1, "5\n14\n23\n");
  Matrix sol;
  bool hasSolution = solve(sol, lhs, rhs);

  ASSERT_TRUE(hasSolution);
  ASSERT_EQ(sol, makeMatrix(1, "1\n2\n0\n0"));
}

TEST(Matrix, Solve3) {
  Matrix lhs = makeMatrix(1, "2");
  Matrix rhs = makeMatrix(2, "-1 8");
  Matrix sol;
  bool hasSolution = solve(sol, lhs, rhs);

  ASSERT_TRUE(hasSolution);
  ASSERT_EQ(sol, makeMatrix(2, "-1/2\n4\n"));
}

TEST(Matrix, Print) {
  const char* str =
	"  13  -171    29    41    37    17\n"
	"  11    61  -278   131    19    23\n"
	"  -3    53   123  -317    41     7\n"
	"  11    97    23    47  -297    41\n"
	"  13    19    11    37    61  -143\n";
  ostringstream pr;
  pr << makeMatrix(6, str);
  ASSERT_EQ(pr.str(), str);
}

TEST(Matrix, Determinant) {
  Matrix mat1 = makeMatrix
	(3,
	 " 4  1  1\n"
	 "-1 -2  1\n"
	 " 2 -1  0\n");
  ASSERT_EQ(determinant(mat1), 11);

  Matrix mat2 = makeMatrix
	(2,
	 " 0  3\n"
	 " 2  0\n");
  ASSERT_EQ(determinant(mat2), -6);

  Matrix mat3 = makeMatrix
	(3,
	 " 0  1    0\n"
	 " 2  0  100\n"
	 " 0  0    7\n");
  ASSERT_EQ(determinant(mat3), -14);

  Matrix mat4 = makeMatrix
	(1, "-2\n");
  ASSERT_EQ(determinant(mat4), -2);
}

TEST(Matrix, IsParallelogram) {
  // zeroes and variations of number of pointsx
  for (size_t dim = 0; dim < 5; ++dim) {
	ASSERT_FALSE(isParallelogram(Matrix(0, dim)));
	ASSERT_FALSE(isParallelogram(Matrix(3, dim)));
	ASSERT_TRUE(isParallelogram(Matrix(4, dim)));
	ASSERT_FALSE(isParallelogram(Matrix(5, dim)));
  }

  // permutations
  ASSERT_TRUE(isParallelogram(makeMatrix(2, "1 1\n 2 3\n 3 1\n 4 3\n")));
  ASSERT_TRUE(isParallelogram(makeMatrix(2, "2 3\n 1 1\n 3 1\n 4 3\n")));
  ASSERT_TRUE(isParallelogram(makeMatrix(2, "2 3\n 1 1\n 4 3\n 3 1\n")));
  ASSERT_TRUE(isParallelogram(makeMatrix(2, "2 3\n 4 3\n 1 1\n 3 1\n")));
  ASSERT_TRUE(isParallelogram(makeMatrix(2, "4 3\n 2 3\n 1 1\n 3 1\n")));
  ASSERT_FALSE(isParallelogram(makeMatrix(2, "4 3\n 2 3\n 1 1\n 3 3\n")));

  // higher dimension
  ASSERT_TRUE(isParallelogram(makeMatrix(3,"1 1 4\n 2 3 6\n 3 1 7\n 4 3 9\n")));
  ASSERT_FALSE(isParallelogram(makeMatrix(3,"1 1 5\n2 3 6\n 3 1 7\n 4 3 9\n")));

  // lower dimension
  ASSERT_TRUE(isParallelogram(makeMatrix(1, "1\n 2\n 3\n 4\n")));
  ASSERT_FALSE(isParallelogram(makeMatrix(1, "1\n 1\n 3\n 4\n")));
}

TEST(Matrix, GetParallelogramArea) {
  ASSERT_EQ(0, getParallelogramAreaSq(makeMatrix(1, "1\n1\n1\n1")));
  ASSERT_EQ(0, getParallelogramAreaSq(makeMatrix(2, "1 2\n1 2\n3 5\n3 5")));
  ASSERT_EQ(1, getParallelogramAreaSq(makeMatrix(2, "0 0\n0 1\n1 0\n1 1")));
  ASSERT_EQ(4, getParallelogramAreaSq(makeMatrix(2, "0 0\n0 2\n1 0\n1 2")));
  ASSERT_EQ(4, getParallelogramAreaSq(makeMatrix(2, "1 2\n1 4\n2 2\n2 4")));
  ASSERT_EQ(4, getParallelogramAreaSq
			(makeMatrix(3, "1 2 1\n1 4 1\n2 2 1\n2 4 1")));

  ASSERT_EQ(44, getParallelogramAreaSq
			(makeMatrix(3, "1 2 0\n1 4 2\n2 2 3\n2 4 5")));

  ASSERT_EQ(19277, getParallelogramAreaSq
			(makeMatrix(3, "5 -1 6\n 13 -5 5\n -14 5 -7\n -6 1 -8")));
  ASSERT_EQ(2580644, getParallelogramAreaSq
			(makeMatrix(4, "-17 -15 -16 29\n -16 7 -24 18\n"
						"-2 -10 27 -14\n -1 12 19 -25")));
}
