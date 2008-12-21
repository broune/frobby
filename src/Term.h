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
#ifndef TERM_GUARD
#define TERM_GUARD

// This file contains a number of functions designed to manipulate
// exponent vectors represented as an array of Exponents along with a
// length. It also contains a class that wraps these functions.

// Sets res equal to the product of a and b.
inline void product(Exponent* res,
		    const Exponent* a, const Exponent* b,
		    size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    res[var] = a[var] + b[var];
}

// Sets res equal to a : b.
//
// a : b is read as "a colon b", and it is defined as lcm(a, b) / b.
inline void colon(Exponent* res,
				  const Exponent* a, const Exponent* b,
				  size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] > b[var])
      res[var] = a[var] - b[var];
    else
      res[var] = 0;
  }
}

// dualOf encodes an irreducible ideal as a term, and the dual of that in
// point is a principal ideal of which res will be the generator. This
// requires that dualOf divides point, as otherwise that dual is not defined.
inline void encodedDual(Exponent* res,
                        const Exponent* dualOf, const Exponent* point,
                        size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  ASSERT(dualOf != 0 || varCount == 0);
  ASSERT(point != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    ASSERT(dualOf[var] <= point[var]);
    if (dualOf[var] != 0)
      res[var] = point[var] - dualOf[var] + 1;
    else
      res[var] = 0;
  }
}

inline void decrement(Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
	if (a[var] > 0)
	  a[var] -= 1;
}

// Sets res equal to the greatest common divisor of a and b.
inline void gcd(Exponent* res,
				const Exponent* a, const Exponent* b,
				size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] < b[var])
      res[var] = a[var];
    else
      res[var] = b[var];
  }
}

// Sets res equal to the least commom multiple of a and b.
inline void lcm(Exponent* res,
				const Exponent* a, const Exponent* b,
				size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] > b[var])
      res[var] = a[var];
    else
      res[var] = b[var];
  }
}

// Returns true iff a divides b.
inline bool divides(const Exponent* a, const Exponent* b, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] > b[var])
      return false;
  return true;
}

// Returns true iff a dominates b.
//
// a dominates b if a[var] >= b[var] for all var, i.e. if b divides a.
inline bool dominates(const Exponent* a, const Exponent* b,
		      size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] < b[var])
      return false;
  return true;
}


// Returns true iff a strictly divides b.
//
// a strictly divides b if a * gcd(a, x_1...x_n) divides b, i.e. if
// the exponents of a are strictly larger than those of b except that
// it is allowed for both of a[var] and b[var] to be 0.
inline bool strictlyDivides(const Exponent* a, const Exponent* b,
			    size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] >= b[var] && a[var] > 0)
      return false;
  return true;
}

// Sets the exponent vector of res to all zeroes, i.e. sets res equal
// to 1.
inline void setToIdentity(Exponent* res, size_t varCount) {
  ASSERT(res != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    res[var] = 0;
}

// Returns true iff the exponent vector of term is all zeroes,
// i.e. iff a is equal to 1.
inline bool isIdentity(const Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] != 0)
      return false;
  return true;
}

// Returns true iff a[var] <= 1 for all var.
inline bool isSquareFree(const Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] >= 2)
      return false;
  return true;
}

// Returns var such that a[var] >= a[i] for all i.
inline size_t getFirstMaxExponent(const Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  size_t max = 0;
  for (size_t var = 1; var < varCount; ++var)
    if (a[max] < a[var])
      max = var;
  return max;
}

// Returns the least integer var such that a[var] is non-zero.
// Returns varCount if no such var exists.
inline size_t getFirstNonZeroExponent(const Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] != 0)
      return var;
  return varCount;
}

// Returns the number of integers var such that a[var] is non-zero.
inline size_t getSizeOfSupport(const Exponent* a, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  size_t size = 0;
  for (size_t var = 0; var < varCount; ++var)
    if (a[var] != 0)
      ++size;
  return size;
}

inline bool hasSameSupport(const Exponent* a, const Exponent* b,
						   size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
	if (a[var] == 0) {
	  if (b[var] != 0)
		return false;
	} else {
	  ASSERT(a[var] != 0);
	  if (b[var] == 0)
		return false;
	}
  }
  return true;
}

// Defines lexicographic order on exponents.
//  Returns something < 0 if a < b.
//  Returns 0 if a = b.
//  Returns something > 0 if a > b.
//
// For example (0,0) < (0,1) < (1,0).
inline int lexCompare(const Exponent* a, const Exponent* b,
					  size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] == b[var])
      continue;

    if (a[var] < b[var])
      return -1;
    else
      return 1;
  }
  return 0;
}

inline bool equals(const Exponent* a, const Exponent* b, size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var)
	if (a[var] != b[var])
	  return false;
  return true;
}

// Defines reverse lexicographic order on exponents.
inline int reverseLexCompare(const Exponent* a, const Exponent* b,
		   size_t varCount) {
  ASSERT(a != 0 || varCount == 0);
  ASSERT(b != 0 || varCount == 0);
  for (size_t var = 0; var < varCount; ++var) {
    if (a[var] == b[var])
      continue;

    if (a[var] > b[var])
      return -1;
    else
      return 1;
  }
  return 0;
}

// Writes e to file.
inline void print(FILE* file, const Exponent* e, size_t varCount) {
  ASSERT(e != 0 || varCount == 0);
  fputc('(', file);

  for (size_t var = 0; var < varCount; ++var) {
    if (var != 0)
      fputs(", ", file);
    fprintf(file, "%lu", (unsigned long)e[var]);
  }

  fputc(')', file);
}

// Term represents a product of variables and does NOT include a
// coefficient. Thus Monomial would be a more fitting name for Term,
// but Term appears so much that it would be cumbersome to type
// Monomial all the time.
//
// Term is mostly a wrapper for the functions above.
class Term {
 public:
  Term(): _exponents(0), _varCount(0) {}
  Term(const Term& term) {initialize(term._exponents, term._varCount);}
  Term(const Exponent* exponents, size_t varCount) {
    initialize(exponents, varCount);
  }
  
  // Term is initialized to the identity.
  Term(size_t varCount):
    _varCount(varCount) {
    if (varCount > 0) {
      _exponents = allocate(varCount);
      setToIdentity();
    } else
      _exponents = 0;
  }

  ~Term() {deallocate(_exponents, _varCount);}

  operator Exponent*() {return _exponents;}
  operator const Exponent*() const {return _exponents;}

  Exponent* begin() {return _exponents;}
  const Exponent* begin() const {return _exponents;}

  Exponent* end() {return _exponents + _varCount;}
  const Exponent* end() const {return _exponents + _varCount;}

  size_t getVarCount() const {return _varCount;}

  // We need all these versions to make everything work out on
  // different platforms.
  Exponent operator[](int offset) const {
	ASSERT(0 <= offset);
    ASSERT((unsigned int)offset < _varCount);
    return _exponents[offset];
  }
  Exponent operator[](unsigned int offset) const {
    ASSERT(offset < _varCount);
    return _exponents[offset];
  }
  Exponent operator[](unsigned long offset) const {
    ASSERT(offset < _varCount);
    return _exponents[offset];
  }

  Exponent& operator[](int offset) {
	ASSERT(0 <= offset);
    ASSERT((unsigned int)offset < _varCount);
    return _exponents[offset];
  }
  Exponent& operator[](unsigned int offset) {
    ASSERT(offset < _varCount);
    return _exponents[offset];
  }
  Exponent& operator[](unsigned long offset) {
    ASSERT(offset < _varCount);
    return _exponents[offset];
  }

  bool operator==(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return (*this) == term._exponents;
  }

  bool operator==(const Exponent* term) const {
	return equals(_exponents, term, _varCount);
  }

  Term& operator=(const Term& term) {
    if (_varCount != term._varCount) {
	  Exponent* newBuffer = allocate(term._varCount);
	  deallocate(_exponents, _varCount);
	  _exponents = newBuffer;
	  _varCount = term._varCount;
	}

    ASSERT(_varCount == term._varCount);
    return (*this) = term._exponents;
  }

  Term& operator=(const Exponent* exponents) {
    copy(exponents, exponents + _varCount, _exponents);
    return *this;
  }
  
  bool divides(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return ::divides(_exponents, term._exponents, _varCount);
  }

  bool divides(const Exponent* term) const {
    return ::divides(_exponents, term, _varCount);
  }

  bool dominates(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return ::dominates(_exponents, term._exponents, _varCount);
  }

  bool dominates(const Exponent* term) const {
    return ::dominates(_exponents, term, _varCount);
  }

  bool strictlyDivides(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return ::strictlyDivides(_exponents, term._exponents, _varCount);
  }

  bool strictlyDivides(const Exponent* term) const {
    return ::strictlyDivides(_exponents, term, _varCount);
  }

  void lcm(const Term& a, const Term& b, int position) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    ::lcm(_exponents + position,
		  a._exponents + position,
		  b._exponents + position,
		  _varCount - position);
  }

  void lcm(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    ::lcm(_exponents, a._exponents, b._exponents, _varCount);
  }

  void lcm(const Exponent* a, const Exponent* b) {
    ::lcm(_exponents, a, b, _varCount);
  }

  void gcd(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    gcd(a._exponents, b._exponents);
  }

  void gcd(const Exponent* a, const Exponent* b) {
    ::gcd(_exponents, a, b, _varCount);
  }

  bool operator<(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return ::lexCompare(_exponents, term._exponents, _varCount) < 0;
  }

  bool operator<(const Exponent* term) const {
    return ::lexCompare(_exponents, term, _varCount) < 0;
  }

  void product(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    ::product(_exponents, a._exponents, b._exponents, _varCount);
  }

  void product(const Exponent* a, const Exponent* b) {
    ::product(_exponents, a, b, _varCount);
  }

  void setToIdentity() {
    ::setToIdentity(_exponents, _varCount);
  }

  bool isIdentity() const {
    return ::isIdentity(_exponents, _varCount);
  }

  bool isSquareFree() const {
    return ::isSquareFree(_exponents, _varCount);
  }

  size_t getFirstNonZeroExponent() const {
    return ::getFirstNonZeroExponent(_exponents, _varCount);
  }

  size_t getFirstMaxExponent() const {
    return ::getFirstMaxExponent(_exponents, _varCount);
  }

  size_t getSizeOfSupport() const {
    return ::getSizeOfSupport(_exponents, _varCount);
  }

  bool hasSameSupport(const Term& a) const {
	ASSERT(_varCount == a._varCount);
	return hasSameSupport(a._exponents);
  }

  bool hasSameSupport(const Exponent* a) const {
	return ::hasSameSupport(_exponents, a, _varCount);
  }

  void colon(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    colon(a._exponents, b._exponents);
  }

  void colon(const Exponent* a, const Exponent* b) {
    ::colon(_exponents, a, b, _varCount);
  }

  void encodedDual(const Term& dualOf, const Term& point) {
    ASSERT(_varCount == dualOf._varCount);
    ASSERT(_varCount == point._varCount);
    encodedDual(dualOf._exponents, point._exponents);
  }

  void encodedDual(const Exponent* dualOf, const Exponent* point) {
    ::encodedDual(_exponents, dualOf, point, _varCount);
  }

  void decrement() {
	::decrement(_exponents, _varCount);
  }

  void swap(Term& term) {
    std::swap(_varCount, term._varCount);

    Exponent* tmp = _exponents;
    _exponents = term._exponents;
    term._exponents = tmp;
  }

  void reset(size_t newVarCount) {
    if (newVarCount != _varCount) {
	  Exponent* newBuffer = allocate(newVarCount);

      deallocate(_exponents, _varCount);
      _varCount = newVarCount;
      _exponents = newBuffer;
    }
    setToIdentity();  
  }

  void clear() {
    deallocate(_exponents, _varCount);
    _exponents = 0;
    _varCount = 0;
  }

  void print(FILE* file) const {
    ::print(file, _exponents, _varCount);
  }

  // A predicate that sorts according to lexicographic order.
  class LexComparator {
  public:
    LexComparator(size_t varCount): _varCount(varCount) {}

    bool operator()(const Term& a, const Term& b) {
      ASSERT(_varCount == a._varCount);
      ASSERT(_varCount == b._varCount);
      return a < b;
    }

    bool operator()(const Exponent* a, const Exponent* b) const {
      return ::lexCompare(a, b, _varCount) < 0;
    }

  private:
    size_t _varCount;
  };

  // A predicate that sorts according to reverse lexicographic order.
  class ReverseLexComparator {
  public:
    ReverseLexComparator(size_t varCount): _varCount(varCount) {}

    bool operator()(const Exponent* a, const Exponent* b) const {
      return ::reverseLexCompare(a, b, _varCount) < 0;
    }

  private:
    size_t _varCount;
  };

  // A predicate that sorts terms in weakly ascending order according
  // to degree of the specified variable. There is no tie-breaker for
  // different terms with the same degree of that variable.
  class AscendingSingleDegreeComparator {
  public:
    AscendingSingleDegreeComparator(size_t variable, size_t varCount):
      _variable(variable),
      _varCount(varCount) {
      ASSERT(variable < varCount);
    }

    bool operator()(const Term& a, const Term& b) {
      ASSERT(_varCount == a._varCount);
      ASSERT(_varCount == b._varCount);
      return a[_variable] < b[_variable];
    }

    bool operator()(const Exponent* a, const Exponent* b) const {
      return a[_variable] < b[_variable];
    }

  private:
    size_t _variable;
    size_t _varCount;
  };

  // Reverse sorted order of AscendingSingleDegreeComparator.
  class DescendingSingleDegreeComparator {
  public:
    DescendingSingleDegreeComparator(size_t variable, size_t varCount):
      _variable(variable),
      _varCount(varCount) {
      ASSERT(variable < varCount);
    }

    bool operator()(const Term& a, const Term& b) {
      ASSERT(_varCount == a._varCount);
      ASSERT(_varCount == b._varCount);
      return a[_variable] > b[_variable];
    }

    bool operator()(const Exponent* a, const Exponent* b) const {
      return a[_variable] > b[_variable];
    }

  private:
    size_t _variable;
    size_t _varCount;
  };

  class EqualsPredicate {
  public:
    EqualsPredicate(size_t varCount): _varCount(varCount) {}

    bool operator()(const Term& a, const Term& b) {
      ASSERT(_varCount == a._varCount);
      ASSERT(_varCount == b._varCount);
      return (*this)(a.begin(), b.begin());
    }

    bool operator()(const Exponent* a, const Exponent* b) const {
      for (size_t var = 0; var < _varCount; ++var)
		if (a[var] != b[var])
		  return false;
      return true;
    }

  private:
    size_t _varCount;
  };

 private:
  static Exponent* allocate(size_t size);
  static void deallocate(Exponent* p, size_t size);
  
  void initialize(const Exponent* exponents, size_t varCount) {
    if (varCount > 0) {
      ASSERT(exponents != 0);
      _exponents = allocate(varCount);
      copy(exponents, exponents + varCount, _exponents);
    } else
      _exponents = 0;
    _varCount = varCount;
  }

  Exponent* _exponents;
  size_t _varCount;
};

namespace std {
  // This allows STL to swap terms more efficiently.
  template<> inline void swap<Term>(Term& a, Term& b) {
    a.swap(b);
  }
}

#endif
