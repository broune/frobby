/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef SQUARE_FREE_IDEAL_GUARD
#define SQUARE_FREE_IDEAL_GUARD

#include "VarNames.h"
#include "RawSquareFreeIdeal.h"
class BigIdeal;

class SquareFreeIdeal {
 public:
  SquareFreeIdeal();
  SquareFreeIdeal(const SquareFreeIdeal& ideal, size_t capacity);
  SquareFreeIdeal(const BigIdeal& ideal);
  ~SquareFreeIdeal();

  SquareFreeIdeal& operator=(const BigIdeal& ideal);

  void minimize();
  void renameVars(const VarNames& names);
  void insert(Word* term);
  void insertIdentity();

  /** Returns false if the strings in term are not all "", "1" or "0". */
  bool insert(const std::vector<std::string>& term);

  /** Change 0 exponents into 1 and vice versa. */
  void swap01Exponents() {_ideal->swap01Exponents();}

  void clear();
  void swap(SquareFreeIdeal& ideal);
  void reserve(size_t capacity);

  const VarNames& getNames() const {return _names;}
  size_t getVarCount() const;
  size_t getGeneratorCount() const;
  size_t getCapacity() const {return _capacity;}

  Word* back() {return _ideal->back();}
  const Word* back() const {return _ideal->back();}

  typedef RawSquareFreeIdeal::iterator iterator;
  iterator begin() {return _ideal->begin();}
  iterator end() {return _ideal->end();}

  typedef RawSquareFreeIdeal::const_iterator const_iterator;
  const_iterator begin() const {return _ideal->begin();}
  const_iterator end() const {return _ideal->end();}

  const RawSquareFreeIdeal* getRawIdeal() const {return _ideal;}
  RawSquareFreeIdeal* getRawIdeal() {return _ideal;}

 private:
  SquareFreeIdeal(const SquareFreeIdeal&); // not available
  SquareFreeIdeal& operator=(const SquareFreeIdeal&); // not available

  VarNames _names;
  RawSquareFreeIdeal* _ideal;
  size_t _capacity;
};



inline size_t SquareFreeIdeal::getVarCount() const {
  ASSERT(_ideal != 0);
  ASSERT(_names.getVarCount() == _ideal->getVarCount());
  return _names.getVarCount();
}

inline size_t SquareFreeIdeal::getGeneratorCount() const {
  ASSERT(_ideal != 0);
  return _ideal->getGeneratorCount();
}

#endif
