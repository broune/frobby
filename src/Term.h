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

#include <ostream>

/** Term represents a product of variables which does not include a
 coefficient. This concept is also sometimes called a monomial or
 power product.

 A term is represented as an array of Exponent and an integer
 indicating the length of the array, i.e. the number of variables. It
 is sometimes desirable to separate the length from the array,
 e.g. when representing an array of terms all of the same length, such
 as in representing the generators of a monomial ideal, in which case
 representing the length once for each generator would be
 wasteful. Thus Term has static versions of most methods where the
 number of variables is a separate parameter. In these cases it is
 allowed for the exponent array pointer to be null (i.e. equal to 0)
 if the length is zero.

 Most methods on Term are inline to avoid function call overhead. This
 is significant because these methods tend to be called in the
 innermost loops of monomial ideal algorithms.

 @todo Move the inline code out of the class declaration and add them
 as inline below the class declaration.

 @todo Duplicate the comments for overloads using the copydoc Doxygen
 command.

 @todo Rename term.h to Term.h.
*/
class Term {
 public:
 Term(): _exponents(0), _varCount(0) {}
  Term(const Term& term) {initialize(term._exponents, term._varCount);}
  Term(const Exponent* exponents, size_t varCount) {
    initialize(exponents, varCount);
  }
  
  /** This object is initialized to the identity, i.e.\ the exponent
	  vector is the zero vector.
  */
 Term(size_t varCount):
  _varCount(varCount) {
	if (varCount > 0) {
	  _exponents = allocate(varCount);
      setToIdentity();
    } else
      _exponents = 0;
  }
  
  /** Accepts a whitespace-separated list of integers as exponent
	  vector.
  */
  Term(const string& str);

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

  bool operator!=(const Term& term) const {
	return !(*this == term);
  }

  bool operator!=(const Exponent* term) const {
	return !(*this == term);
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
  
  /** Returns whether a divides b. */
  inline static bool divides(const Exponent* a, const Exponent* b, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	ASSERT(b != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] > b[var])
		return false;
	return true;
  }

  bool divides(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return divides(_exponents, term._exponents, _varCount);
  }

  bool divides(const Exponent* term) const {
    return divides(_exponents, term, _varCount);
  }

  /** Returns whether a dominates b, i.e.\ whether b divides a.
   */
  inline static bool dominates(const Exponent* a, const Exponent* b,
							   size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	ASSERT(b != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] < b[var])
		return false;
	return true;
  }

  bool dominates(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return dominates(_exponents, term._exponents, _varCount);
  }

  bool dominates(const Exponent* term) const {
    return dominates(_exponents, term, _varCount);
  }

  /** Returns whether a strictly divides b. \f$a\f$ strictly divides
	  \f$b\f$ if \f$a * gcd(a, x_1...x_n)\f$ divides \f$b\f$, i.e.\ if,
	  for each $i$, \f$u_i<b_i\f$ or $v_i=0$ where $a=x^u$ and $b=x^v$.
  */
  inline static bool strictlyDivides(const Exponent* a, const Exponent* b,
									 size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	ASSERT(b != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] >= b[var] && a[var] > 0)
		return false;
	return true;
  }

  bool strictlyDivides(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return strictlyDivides(_exponents, term._exponents, _varCount);
  }

  bool strictlyDivides(const Exponent* term) const {
    return strictlyDivides(_exponents, term, _varCount);
  }

  /** Sets res equal to the least commom multiple of a and b. */
  inline static void lcm(Exponent* res,
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

  void lcm(const Term& a, const Term& b, int position) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    lcm(_exponents + position,
		a._exponents + position,
		b._exponents + position,
		_varCount - position);
  }

  void lcm(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    lcm(_exponents, a._exponents, b._exponents, _varCount);
  }

  void lcm(const Exponent* a, const Exponent* b) {
    lcm(_exponents, a, b, _varCount);
  }

  /** Sets res equal to the greatest common divisor of a and b. */
  inline static void gcd(Exponent* res,
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

  void gcd(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    gcd(a._exponents, b._exponents);
  }

  void gcd(const Exponent* a, const Exponent* b) {
    gcd(_exponents, a, b, _varCount);
  }

  bool operator<(const Term& term) const {
    ASSERT(_varCount == term._varCount);
    return lexCompare(_exponents, term._exponents, _varCount) < 0;
  }

  bool operator<(const Exponent* term) const {
    return lexCompare(_exponents, term, _varCount) < 0;
  }

  /** Sets res equal to the product of a and b. */
  inline static void product(Exponent* res,
							 const Exponent* a, const Exponent* b,
							 size_t varCount) {
	ASSERT(res != 0 || varCount == 0);
	ASSERT(a != 0 || varCount == 0);
	ASSERT(b != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  res[var] = a[var] + b[var];
  }

  /** Set this object equal to the product of a and b. */
  void product(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    product(_exponents, a._exponents, b._exponents, _varCount);
  }

  void product(const Exponent* a, const Exponent* b) {
    product(_exponents, a, b, _varCount);
  }

  /** Set res equal to \f$1=x^{(0,\ldots,0)}\f$, i.e.\ set each entry
	  of res equal to 0.
  */
  inline static void setToIdentity(Exponent* res, size_t varCount) {
	ASSERT(res != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  res[var] = 0;
  }

  void setToIdentity() {
    setToIdentity(_exponents, _varCount);
  }

  /** Returns whether a is 1, i.e.\ whether all entries of a are 0.
   */
  inline static bool isIdentity(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] != 0)
		return false;
	return true;
  }

  bool isIdentity() const {
    return isIdentity(_exponents, _varCount);
  }

  /** Returns whether a is square free, i.e.\ \f$v_i\leq 1\f$ for each
	  \f$i\f$ where $a=x^v$.
  */
  inline static bool isSquareFree(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] >= 2)
		return false;
	return true;
  }

  bool isSquareFree() const {
    return isSquareFree(_exponents, _varCount);
  }

  /** Returns least var such that a[var] is non-zero. Returns varCount
	  if the entries of a are all zero.
  */
  inline static size_t getFirstNonZeroExponent(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] != 0)
		return var;
	return varCount;
  }

  size_t getFirstNonZeroExponent() const {
    return getFirstNonZeroExponent(_exponents, _varCount);
  }

  /** Returns a median element of the set of var's such that a[var] is
	  non-zero. Returns varCount is the entries of a are all zero.
  */
  inline static size_t getMiddleNonZeroExponent(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	size_t nonZeroOffset = getSizeOfSupport(a, varCount) / 2;
	for (size_t var = 0; var < varCount; ++var) {
	  if (a[var] != 0) {
		if (nonZeroOffset == 0)
		  return var;
		--nonZeroOffset;
	  }
	}

	ASSERT(isIdentity(a, varCount));
	return varCount;
  }

  size_t getMiddleNonZeroExponent() const {
    return getMiddleNonZeroExponent(_exponents, _varCount);
  }

  /** Returns a var such that a[var] >= a[i] for all i. */
  inline static size_t getFirstMaxExponent(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	size_t max = 0;
	for (size_t var = 1; var < varCount; ++var)
	  if (a[max] < a[var])
		max = var;
	return max;
  }

  size_t getFirstMaxExponent() const {
    return getFirstMaxExponent(_exponents, _varCount);
  }

  /** Returns the number of variables \f$x_i\f$ such that \f$x_i\f$
	  divides \f$a\f$.
  */
  inline static size_t getSizeOfSupport(const Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	size_t size = 0;
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] != 0)
		++size;
	return size;
  }

  size_t getSizeOfSupport() const {
    return getSizeOfSupport(_exponents, _varCount);
  }

  /** Returns whether \f$x_i|a\Leftrightarrow x_i|b\f$ for every
	  variable \f$x_i\f$.
  */
  inline static bool hasSameSupport(const Exponent* a, const Exponent* b,
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

  bool hasSameSupport(const Term& a) const {
	ASSERT(_varCount == a._varCount);
	return hasSameSupport(a._exponents);
  }

  bool hasSameSupport(const Exponent* a) const {
	return hasSameSupport(_exponents, a, _varCount);
  }

  /** Sets res equal to \f$a : b\f$.
	  \f$a : b\f$ is read as "a colon b", and it is defined as \f$lcm(a, b)
	  / b\f$.
  */
  inline static void colon(Exponent* res,
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

  void colon(const Term& a, const Term& b) {
    ASSERT(_varCount == a._varCount);
    ASSERT(_varCount == b._varCount);
    colon(a._exponents, b._exponents);
  }

  void colon(const Exponent* a, const Exponent* b) {
    colon(_exponents, a, b, _varCount);
  }

  /** The parameter dualOf is interpreted to encode an irreducible
	  ideal, and the dual of that reflected in point is a principal
	  ideal. The generated of this ideal is written to res. This requires
	  that dualOf divides point, as otherwise that dual is not defined.
  */
  inline static void encodedDual(Exponent* res,
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

  void encodedDual(const Term& dualOf, const Term& point) {
    ASSERT(_varCount == dualOf._varCount);
    ASSERT(_varCount == point._varCount);
    encodedDual(dualOf._exponents, point._exponents);
  }

  void encodedDual(const Exponent* dualOf, const Exponent* point) {
    encodedDual(_exponents, dualOf, point, _varCount);
  }

  /** Decrements each positive entry of a by one. */
  inline static void decrement(Exponent* a, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] > 0)
		a[var] -= 1;
  }

  void decrement() {
	decrement(_exponents, _varCount);
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

  /** Writes e to file in a format suitable for debug output. */
  static void print(FILE* file, const Exponent* e, size_t varCount);

  /** Writes e to out in a format suitable for debug output. */
  static void print(ostream& out, const Exponent* e, size_t varCount);

  void print(FILE* file) const {
    print(file, _exponents, _varCount);
  }

  void print(ostream& out) const {
    print(out, _exponents, _varCount);
  }

  /** Indicates how a relates to b according to the lexicographic term
	  order where \f$x_1>\cdots>x_n\f$. Returns -1 if a < b, returns 0 if a
	  = b and returns 1 if a > b. As an example \f$x^(0,0) < x^(0,1) <
	  x^(1,0)\f$.
  */
  inline static int lexCompare(const Exponent* a, const Exponent* b,
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
      return lexCompare(a, b, _varCount) < 0;
    }

  private:
    size_t _varCount;
  };

  /** Indicates how a relates to b according to the reverse
	  lexicographic term order where \f$x_1<\cdots<x_n\f$. Returns -1 if a
	  < b, returns 0 if a = b and returns 1 if a > b. As an example
	  \f$x^(0,0) < x^(1,0) < x^(0,1)\f$.
  */
  // Defines reverse lexicographic order on exponents.
  inline static int reverseLexCompare(const Exponent* a, const Exponent* b,
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

  // A predicate that sorts according to reverse lexicographic order.
  class ReverseLexComparator {
  public:
  ReverseLexComparator(size_t varCount): _varCount(varCount) {}

    bool operator()(const Exponent* a, const Exponent* b) const {
      return reverseLexCompare(a, b, _varCount) < 0;
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

  /** Returns whether the entries of a are equal to the entries of b. */
  inline static bool equals(const Exponent* a, const Exponent* b, size_t varCount) {
	ASSERT(a != 0 || varCount == 0);
	ASSERT(b != 0 || varCount == 0);

	for (size_t var = 0; var < varCount; ++var)
	  if (a[var] != b[var])
		return false;
	return true;
  }

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

inline ostream& operator<<(ostream& out, const Term& term) {
  term.print(out);
  return out;
}

#endif
