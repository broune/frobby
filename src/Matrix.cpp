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

#include "BigIntVector.h"
#include "ColumnPrinter.h"

#include <utility>
#include <sstream>

namespace {
  /** Multiplies column col of mat by a positive fractional number
	  such that that column becomes integral and primitive. A vector is
	  primitive if the gcd of its entries is 1. */
  void makeColumnIntegralPrimitive(Matrix& mat, size_t col) {
	ASSERT(col < mat.getColCount());
	if (mat.getRowCount() == 0)
	  return;

	// Obtain gcd of numerators and lcm of denominators.
	mpz_class numGcd = mat(0, col).get_num();
	mpz_class denLcm = mat(0, col).get_den();
	for (size_t row = 1; row < mat.getRowCount(); ++row) {
	  mpz_gcd(numGcd.get_mpz_t(), numGcd.get_mpz_t(),
			  mat(row, col).get_num_mpz_t());
	  mpz_lcm(denLcm.get_mpz_t(), denLcm.get_mpz_t(),
			  mat(row, col).get_den_mpz_t());
	}
	ASSERT(numGcd > 0);
	ASSERT(denLcm > 0);

	for (size_t row = 0; row < mat.getRowCount(); ++row) {
	  mat(row, col) /= numGcd;
	  mat(row, col) *= denLcm;

	  ASSERT(mat(row, col).get_den() == 1);
	}
  }
}

Matrix::Matrix(size_t rowCount, size_t colCount):
  _rowCount(rowCount), _colCount(colCount), _entries(rowCount * colCount) {
}

void Matrix::resize(size_t rowCount, size_t colCount) {
  if (rowCount == getRowCount() && colCount == getColCount())
	return;
  Matrix mat(rowCount, colCount);

  size_t copyRowCount = std::min(rowCount, getRowCount());
  size_t copyColCount = std::min(colCount, getColCount());
  for (size_t row = 0; row < copyRowCount; ++row)
	for (size_t col = 0; col < copyColCount; ++col)
	  mat(row, col) = (*this)(row, col);
  swap(mat);
}

void Matrix::swap(Matrix& mat) {
  using std::swap;

  _entries.swap(mat._entries);
  swap(_rowCount, mat._rowCount);
  swap(_colCount, mat._colCount);
}

bool operator==(const Matrix& a, const Matrix& b) {
  if (a.getRowCount() != b.getRowCount() ||
	  a.getColCount() != b.getColCount())
	return false;

  for (size_t row = 0; row < a.getRowCount(); ++row)
	for (size_t col = 0; col < a.getColCount(); ++col)
	  if (a(row, col) != b(row, col))
		return false;
  return true;
}

ostream& operator<<(ostream& out, const Matrix& mat) {
  ColumnPrinter pr;
  print(pr, mat);
  pr.print(out);
  return out;
}

void print(FILE* file, const Matrix& mat) {
  ostringstream out;
  out << mat;
  fputs(out.str().c_str(), file);
}

void print(ColumnPrinter& pr, const Matrix& mat) {
  size_t baseCol = pr.getColumnCount();
  for (size_t i = 0; i < mat.getColCount(); ++i)
	pr.addColumn(false);
  for (size_t col = 0; col < mat.getColCount(); ++col)
	for (size_t row = 0; row < mat.getRowCount(); ++row)
	  pr[baseCol + col] << mat(row, col) << '\n';
}

void product(Matrix& prod, const Matrix& a, const Matrix& b) {
  ASSERT(a.getColCount() == b.getRowCount());

  prod.resize(a.getRowCount(), b.getColCount());
  for (size_t r = 0; r < a.getRowCount(); ++r) {
	for (size_t c = 0; c < b.getColCount(); ++c) {
	  prod(r, c) = 0;
	  for (size_t i = 0; i < a.getColCount(); ++i)
		prod(r, c) += a(r, i) * b(i, c);
	}
  }
}

void transpose(Matrix& trans, const Matrix& mat) {
  if (&trans == &mat) {
    transpose(trans);
    return;
  }

  trans.resize(mat.getColCount(), mat.getRowCount());
  for (size_t row = 0; row < mat.getRowCount(); ++row)
    for (size_t col = 0; col < mat.getColCount(); ++col)
      trans(col, row) = mat(row, col);
}

void transpose(Matrix& mat) {
  Matrix tmp(mat);
  transpose(mat, tmp);
}


void addMultiplyRow(Matrix& mat, size_t resultRow,
					size_t sourceRow, const mpq_class& mult) {
  ASSERT(resultRow < mat.getRowCount());
  ASSERT(sourceRow < mat.getRowCount());

  for (size_t col = 0; col < mat.getColCount(); ++col)
	mat(resultRow, col) += mat(sourceRow, col) * mult;
}

void multiplyRow(Matrix& mat, size_t row, const mpq_class& mult) {
  for (size_t col = 0; col < mat.getColCount(); ++col)
	mat(row, col) *= mult;
}

void swapRows(Matrix& mat, size_t row1, size_t row2) {
  ASSERT(row1 < mat.getRowCount());
  ASSERT(row2 < mat.getRowCount());

  for (size_t col = 0; col < mat.getColCount(); ++col)
	swap(mat(row1, col), mat(row2, col));
}

bool rowReduce(Matrix& mat) {
  bool permutationOdd = false;

  size_t rowsDone = 0;
  for (size_t pivotCol = 0; pivotCol < mat.getColCount(); ++pivotCol) {
	size_t pivotRow = rowsDone;
	for (; pivotRow < mat.getRowCount(); ++pivotRow)
	  if (mat(pivotRow, pivotCol) != 0)
		break;
	if (pivotRow == mat.getRowCount())
	  continue;

	if (rowsDone != pivotRow) {
	  permutationOdd = !permutationOdd;
	  swapRows(mat, rowsDone, pivotRow);
	}
	pivotRow = rowsDone;
	++rowsDone;

	for (size_t row = pivotRow + 1; row < mat.getRowCount(); ++row) {
	  if (row != pivotRow && mat(row, pivotCol) != 0) {
		addMultiplyRow(mat, row, pivotRow,
					   -mat(row, pivotCol) / mat(pivotRow, pivotCol));
		ASSERT(mat(row, pivotCol) == 0);
	  }
	}
  }

  return permutationOdd;
}

void rowReduceFully(Matrix& mat) {
  rowReduce(mat);

  /// @todo: do back substitution instead.
  size_t pivotCol = 0;
  size_t pivotRow = 0;
  while (pivotRow < mat.getRowCount() &&
		 pivotCol < mat.getColCount()) {
	if (mat(pivotRow, pivotCol) == 0) {
	  ++pivotCol;
	} else {
	  multiplyRow(mat, pivotRow, 1 / mat(pivotRow, pivotCol));
	  ASSERT(mat(pivotRow, pivotCol) == 1);
	  for (size_t row = 0; row < pivotRow; ++row) {
		if (row != pivotRow && mat(row, pivotCol) != 0) {
		  addMultiplyRow(mat, row, pivotRow, -mat(row, pivotCol));
		  ASSERT(mat(row, pivotCol) == 0);
		}
	  }

	  ++pivotRow;
	}
  }
}

void subMatrix(Matrix& sub, const Matrix& mat,
			   size_t rowBegin, size_t rowEnd,
			   size_t colBegin, size_t colEnd) {
  ASSERT(rowBegin <= rowEnd);
  ASSERT(rowEnd <= mat.getRowCount());
  ASSERT(colBegin <= colEnd);
  ASSERT(colEnd <= mat.getColCount());

  if (&sub == &mat) {
	Matrix tmp;
	subMatrix(tmp, mat, rowBegin, rowEnd, colBegin, colEnd);
	sub.swap(tmp);
	return;
  }

  sub.resize(rowEnd - rowBegin, colEnd - colBegin);
  for (size_t row = rowBegin; row < rowEnd; ++row)
	for (size_t col = colBegin; col < colEnd; ++col)
	  sub(row - rowBegin, col - colBegin) = mat(row, col);
}

void copyRow(Matrix& target, size_t targetRow,
	     const Matrix& source, size_t sourceRow) {
  ASSERT(target.getColCount() == source.getColCount());
  ASSERT(targetRow < target.getRowCount());
  ASSERT(sourceRow < source.getRowCount());

  size_t colCount = target.getColCount();
  for (size_t col = 0; col < colCount; ++col)
    target(targetRow, col) = source(sourceRow, col);
}

bool inverse(Matrix& inv, const Matrix& mat) {
  ASSERT(mat.getRowCount() == mat.getColCount());
  size_t size = mat.getRowCount();

  inv = mat;

  // Append identity matrix
  inv.resize(size, size + size);
  for (size_t i = 0; i < size; ++i)
	inv(i, size + i) = 1;

  rowReduceFully(inv);
  if (inv(size - 1, size - 1) == 0)
	return false; // not invertible

  subMatrix(inv, inv, 0, size, size, 2 * size);
  return true;
}

size_t matrixRank(const Matrix& matParam) {
  Matrix mat(matParam);
  rowReduceFully(mat);

  // Find pivots
  size_t rank = 0;
  size_t col = 0;
  size_t row = 0;
  while (row < mat.getRowCount() && col < mat.getColCount()) {
    if (mat(row,  col) == 0) {
      ++col;
    } else {
      ++rank;
      ++row;
    }
  }

  return rank;
}

void nullSpace(Matrix& basis, const Matrix& matParam) {
  Matrix mat(matParam);
  rowReduceFully(mat);

  // Find pivots
  size_t rank = 0;
  vector<char> colHasPivot(mat.getColCount());
  {
    size_t col = 0;
    size_t row = 0;
    while (row < mat.getRowCount() && col < mat.getColCount()) {
      if (mat(row,  col) == 0) {
        ++col;
      } else {
        ++rank;
        colHasPivot[col] = true;
        ++row;
      }
    }
  }

  // Construct basis
  basis.resize(mat.getColCount(), mat.getColCount() - rank);
  size_t nullCol = 0;
  for (size_t col = 0; col < mat.getColCount(); ++col) {
    ASSERT(nullCol <= basis.getColCount());

    if (colHasPivot[col])
      continue;

    ASSERT(nullCol < basis.getColCount());

    size_t row = 0;
    for (size_t nullRow = 0; nullRow < basis.getRowCount(); ++nullRow) {
      if (colHasPivot[nullRow]) {
		basis(nullRow, nullCol) = -mat(row, col);
		++row;
      } else if (nullRow == col)
		basis(nullRow, nullCol) = 1;
      else
		basis(nullRow, nullCol) = 0;
    }

    ++nullCol;
  }
  ASSERT(nullCol == basis.getColCount());

  // Make basis integer
  for (size_t col = 0; col < basis.getColCount(); ++col)
    makeColumnIntegralPrimitive(basis, col);
}

bool solve(Matrix& sol, const Matrix& lhs, const Matrix& rhs) {
  ASSERT(lhs.getRowCount() == rhs.getRowCount());

  // Append lhs|rhs and reduce
  Matrix system = lhs;
  system.resize(system.getRowCount(), system.getColCount() + rhs.getColCount());
  size_t midCol = lhs.getColCount();
  for (size_t col = 0; col < rhs.getColCount(); ++col)
	for (size_t row = 0; row < rhs.getRowCount(); ++row)
	  system(row, midCol + col) = rhs(row, col);

  rowReduceFully(system);

  // Check if system has a solution
  for (size_t row = 0; row < system.getRowCount(); ++row) {
	for (size_t col = 0; col < midCol; ++col)
	  if (system(row, col) != 0)
		goto hasLeftPivot;
	for (size_t col = midCol; col < system.getColCount(); ++col)
	  if (system(row, col) != 0)
		return false;

  hasLeftPivot:;
  }

  // Extract solution
  sol.resize(lhs.getColCount(), rhs.getColCount());
  size_t row = 0;
  for (size_t col = 0; col < midCol; ++col) {
	if (row == system.getRowCount() || system(row, col) == 0) {
	  for (size_t r = 0; r < sol.getColCount(); ++r)
		sol(col, r) = 0;
	} else {
	  ASSERT(system(row, col) == 1);
	  for (size_t r = 0; r < sol.getColCount(); ++r)
		sol(col, r) = system(row, midCol + r);
	  ++row;
	}
  }
  return true;
}

bool hasSameRowSpace(const Matrix& a, const Matrix& b) {
  Matrix trA;
  transpose(trA, a);

  Matrix trB;
  transpose(trB, b);

  return hasSameColSpace(trA, trB);
}

bool hasSameColSpace(const Matrix& a, const Matrix& b) {
  if (a.getRowCount() != b.getRowCount())
    return false;

  // A single row reduction of each of a and b would be a little more
  // efficient.
  Matrix dummy;
  return solve(dummy, a, b) && solve(dummy, b, a);
}

mpq_class determinant(const Matrix& mat) {
  ASSERT(mat.getRowCount() == mat.getColCount());

  Matrix reduced(mat);
  bool permutationOdd = rowReduce(reduced);

  mpq_class det = permutationOdd ? -1 : 1;
  for (size_t i = 0; i < reduced.getRowCount(); ++i)
	det *= reduced(i, i);
  return det;
}

namespace {
  size_t getOppositeZeroRow(const Matrix& mat) {
	// Let a,d and b,c be opposite vertices in a parallelogram. Then
	// and only then b + c == d + a. We return the index of the row opposite
	// to row 0. If the rows of mat are not the vertices of a parallelogram
	// then we return mat.getRowCount().

	if (mat.getRowCount() != 4)
	  return mat.getRowCount();

	mpq_class tmp;
	for (size_t opposite = 1; opposite < 4; ++opposite) {
	  bool isPara = true;
	  for (size_t col = 0; col < mat.getColCount(); ++col) {
		tmp = mat(0, col) + mat(opposite, col);
		for (size_t row = 1; row < 4; ++row)
		  if (row != opposite)
			tmp -= mat(row, col);
		if (tmp != 0) {
		  isPara = false;
		  break;
		}
	  }
	  if (isPara)
		return opposite;
	}
	return mat.getRowCount();
  }
}

bool isParallelogram(const Matrix& mat) {
  return getOppositeZeroRow(mat) != mat.getRowCount();
}

mpq_class getParallelogramAreaSq(const Matrix& mat) {
  ASSERT(isParallelogram(mat));
  size_t opposite = getOppositeZeroRow(mat);

  size_t a;
  for (a = 1; a < 4; ++a)
	if (a != opposite)
	  break;
  ASSERT(a < 4);

  size_t b;
  for (b = a + 1; b < 4; ++b)
	if (b != opposite)
	  break;
  ASSERT(b < 4);

  // Translate to zero and drop the zero and sum vertices.
  Matrix tmp(2, mat.getColCount());
  for (size_t col = 0; col < mat.getColCount(); ++col) {
	tmp(0, col) = mat(a, col) - mat(0, col);
	tmp(1, col) = mat(b, col) - mat(0, col);
  }

  // Now the square of the area is det(tmp*transpose(tmp)).
  Matrix trans;
  transpose(trans, tmp);
  Matrix prod;
  product(prod, tmp, trans);

  return determinant(prod);
}
