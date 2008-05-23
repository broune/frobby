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
#ifndef PARTITION_GUARD
#define PARTITION_GUARD

class Partition {
 public:
  Partition();
  Partition(const Partition& partition);
  ~Partition();

  void reset(size_t size);

  bool join(size_t i, size_t j);

  size_t getSize() const;

  size_t getSetCount() const;

  size_t getSetSize(size_t set) const;

  size_t getRoot(size_t i) const;

  void addToSet(size_t i);

  size_t getSizeOfClassOf(size_t i) const;

  void print(FILE* file) const;

 private:
  // Made private since it is not implemented.
  Partition& operator=(const Partition&);

  int* _partitions;
  size_t _size;
  size_t _capacity;

  size_t _setCount;
};

#endif
