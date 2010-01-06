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
#ifndef DATA_TYPE_GUARD
#define DATA_TYPE_GUARD

#include <vector>

/** The intention of this class is to describe the different kinds of
 mathematical structures that Frobby supports, such as a monomial
 ideal or a polynomial. This is useful for the IO subsystem.

 There is only one instance of each kind, so pointer equality is the
 same as equality.
*/
class DataType {
 public:
  /** Returns the name of the structure. */
  const char* getName() const;

  /** Returns true if this object was returned by getNullType(). */
  bool isNull() const;

  bool operator==(const DataType& type) const;
  bool operator!=(const DataType& type) const;

  /** Returns the one and only instance for null. */
  static const DataType& getNullType();

  /** Returns the one and only instance for monomial ideals. */
  static const DataType& getMonomialIdealType();

  /** Returns the one and only instance for polynomials. */
  static const DataType& getPolynomialType();

  /** Returns the one and only instance for saturated binomial ideals. */
  static const DataType& getSatBinomIdealType();

  /** Returns the one and only instance for monomial ideal lists. */
  static const DataType& getMonomialIdealListType();

  /** Returns a vector of all types except null. */
  static vector<const DataType*> getTypes();

 private:
  /** Private to avoid copies. */
  DataType(const DataType&);

  /** Private to avoid copies. */
  void operator=(const DataType&);

  /** Private to ensure only one instance of each kind. */
  DataType(const char* name);

  const char* _name;
};

#endif
