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
#ifndef BIG_INT_VECTOR_GUARD
#define BIG_INT_VECTOR_GUARD

#include <vector>
#include <ostream>

class BigIntVector {
 public:
 BigIntVector(size_t size): _entries(size) {}

  mpz_class& operator[](size_t i) {
	ASSERT(i < getSize());
	return _entries[i];
  }

  const mpz_class& operator[](size_t i) const {
	ASSERT(i < getSize());
	return _entries[i];
  }

  size_t getSize() const {return _entries.size();}
  void resize(size_t size) {_entries.resize(size);}

 private:
  vector<mpz_class> _entries;
};

bool operator==(const BigIntVector& a, const BigIntVector& b);
ostream& operator<<(ostream& out, const BigIntVector& v);

#endif
