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
#ifndef MATRIX_GUARD
#define MATRIX_GUARD

#include <vector>

class BigIntVector;
class ColumnPrinter;

class Matrix {
 public:
  Matrix(size_t rowCount = 0, size_t colCount = 0);

  size_t getRowCount() const {return _rowCount;}
  size_t getColCount() const {return _colCount;}

  /** Set the number of rows and columns.

   Preserves the value of entries that are still present. New entries
   are initialized to zero. */
  void resize(size_t rowCount, size_t colCount);

  const mpq_class& operator()(size_t row, size_t col) const
  {return _entries[toIndex(row, col)];}
  mpq_class& operator()(size_t row, size_t col)
	{return _entries[toIndex(row, col)];}

  void swap(Matrix& mat);

 private:
  size_t toIndex(size_t row, size_t col) const {
	ASSERT(row < _rowCount);
	ASSERT(col < _colCount);
	ASSERT(_entries.size() == _rowCount * _colCount);
	size_t index = col + row * _colCount;
	ASSERT(index < _entries.size());
	return index;
  }

  size_t _rowCount;
  size_t _colCount;
  vector<mpq_class> _entries;
};

bool operator==(const Matrix& a, const Matrix& b);
ostream& operator<<(ostream& out, const Matrix& mat);

void print(FILE* file, const Matrix& mat);
void print(ColumnPrinter& printer, const Matrix& mat);

/** Sets prod to a * b. */
void product(Matrix& prod, const Matrix& a, const Matrix& b);

/** Sets trans to the transpose of mat. */
void transpose(Matrix& trans, const Matrix& mat);

/** Transpose mat inplace. */
void transpose(Matrix& mat);

/** Adds mult times row sourceRow to row resultRow of mat. */
void addMultiplyRow(Matrix& mat, size_t resultRow,
		    size_t sourceRow, const mpq_class& mult);

/** Multiplies row row with mult. */
void multiplyRow(Matrix& mat, size_t row, const mpq_class& mult);

/** Swaps row row1 and row row2 of mat. */
void swapRows(Matrix& mat, size_t row1, size_t row2);

/** Reduces mat to row-echelon form, i.e. make it upper
	triangular. This does not include making the pivot entries
	1. Returns true if the permutation made of the rows is odd. */
bool rowReduce(Matrix& mat);

/** Returns the rank of mat. */
size_t matrixRank(const Matrix& mat);

/** Reduces mat to reduced row-echelon form, i.e. a row-echelon matrix
 such that no column contains two non-zero entries and all entries are
 zero or one.
*/
void rowReduceFully(Matrix& mat);

/** Sets sub to the sub-matrix of mat with rows in the interval
 [rowBegin, rowEnd) and columns in the interval [colBegin, colEnd). */
void subMatrix(Matrix& sub, const Matrix& mat,
			   size_t rowBegin, size_t rowEnd,
			   size_t colBegin, size_t colEnd);

/** Copies row sourceRow from source to row targetRow of
    target. source and target must have the same number of columns. */
void copyRow(Matrix& target, size_t targetRow,
             const Matrix& source, size_t sourceRow);

/** Sets inv to the inverse of mat.

mat must be a square matrix. Returns true if mat is
invertible. Returns false and leaves inv in some valid but unspecified
state if mat is not invertible. */
bool inverse(Matrix& inv, const Matrix& mat);

/** Sets the columns of basis to a basis of the null space of mat. */
void nullSpace(Matrix& basis, const Matrix& mat);

/** Sets sol to some matrix such that lhs*sol=rhs and returns true if
 such a matrix exists. Otherwise returns false and does not change
 sol. */
bool solve(Matrix& sol, const Matrix& lhs, const Matrix& rhs);

/** Returns true if a and b have the same column space. */
bool hasSameColSpace(const Matrix& a, const Matrix& b);

/** Returns true if a and b have the same row space. */
bool hasSameRowSpace(const Matrix& a, const Matrix& b);

/** Returns the determinant of mat. */
mpq_class determinant(const Matrix& mat);

/** Returns true if the rows of mat are the (4) vertices of a parallelogram. */
bool isParallelogram(const Matrix& mat);

/** Returns the square of the area of the parallelogram whose vertices
	are the 4 rows of mat. Mat must be a parallelogram. The square of
	the are is integer if the entries of mat are integer and otherwise
	it is rational. The are itself can be an irrational number. */
mpq_class getParallelogramAreaSq(const Matrix& mat);

#endif
