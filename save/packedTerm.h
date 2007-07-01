#ifndef _PACKED_TERM_H_GUARD
#define _PACKED_TERM_H_GUARD

#include <iostream>
#include <ostream>
#include "Integer.h"
#include <map>

class PackedTerm;
ostream& operator<<(ostream& out, const PackedTerm& term);

#define PACKED_TERM_NO_SUPPORT

// defining PACKED_TERM_NO_SUPPORT removes the optimization of storing
// the support. This may improve performance or it may degrade it,
// depending on the input.
#ifdef PACKED_TERM_NO_SUPPORT
#define SUPPORT(X)
#else
#define SUPPORT(X) X
#endif

// This class represents a term by packing four 7-bit exponents into a
// single 32 bit word like this: 0aaaaaaa0bbbbbbb0ccccccc0ddddddd
// where aaaaaaa is the exponent at offset 0, bbbbbbb is the exponent
// at offset 1 and so on.  The zeroes waste 1 bit per exponent, but
// they are necessary to check for overflow without resorting to
// assembly code. They also make it possible to check for overflow
// more efficiently than otherwise.
//
// Settings::VariableCount has to be less than or equal to 32 because
// the support of the term is stored in a 32-bit int. Enlarging this
// int or just turning this optimization off (by defining
// PACKED_TERM_NO_SUPPORT) will remove this limitation.
//
// Nitpick: making Settings::VariableCount so astronomically large
// that the degree of a term might possible be unrepresentable in a
// 31-bit unsigned integer will necessitate adding checks for degree
// overflow.
//
// See SimpleTerm for documentation of the interface.
class PackedTerm {
  // divide by 4 and round up
  static const unsigned int wordCount = (Settings::VariableCount + 3) / 4;

 public:
  typedef unsigned int Exponent;

  static Exponent getMaximalExponent() {return 127;}

  struct Uninit {};
  PackedTerm(Uninit) {}

  PackedTerm(): sumOfExps(0) {
    SUPPORT(support = 0);
    for (unsigned int i = 0; i < wordCount; ++i)
      exponents[i] = 0;  
  }

  PackedTerm(Exponent e0, Exponent e1, Exponent e2, Exponent e3 = 0):
    sumOfExps(e0 + e1 + e2 + e3)  {
    ASSERT(Settings::VariableCount >= 4);
    if (e0 > getMaximalExponent() ||
	e1 > getMaximalExponent() ||
	e2 > getMaximalExponent() ||
	e3 > getMaximalExponent())
      overflow();

    exponents[0] = e3 | (e2 << 8) | (e1 << 16) | (e0 << 24);
    for (unsigned int i = 1; i < wordCount; ++i)
      exponents[i] = 0;
    SUPPORT(recalculateSupport());
  }

  template<class iter>
    PackedTerm(iter begin, iter end): sumOfExps(0) {
    SUPPORT(support = 0);

    Exponent* expsIt = exponents;
    int variablesToGo = Settings::VariableCount;
    for (; begin != end && variablesToGo > 0; ++expsIt) {
      int v = 0;
      for (int i = 0; i < 4; ++i) {
	v <<= 8;
	if (variablesToGo == 0)
	  continue;

	if (begin != end) {
	  if (*begin > getMaximalExponent())
	    overflow();
	  
	  v |= *begin;
	  sumOfExps += *begin;
	  ++begin;
	  --variablesToGo;
	}
      }
      *expsIt = v;
    }

    Exponent* expsEnd = exponents + wordCount;
    for (; expsIt != expsEnd; ++expsIt)
      *expsIt = 0;

    SUPPORT(recalculateSupport());
  }

  void multiply(const PackedTerm& a, const PackedTerm& b) {
    SUPPORT(result.support = a.support | b.support);
    sumOfExps = a.sumOfExps + b.sumOfExps;
    for (unsigned int i = 0; i < wordCount; ++i)
      exponents[i] = a.exponents[i] + b.exponents[i];

    // If the sum of the exponents is within bounds, surely the
    // exponents are too. Otherwise, we need to check that they are.
    if (sumOfExps > getMaximalExponent())
      for (unsigned int i = 0; i < wordCount; ++i)
	if (exponents[i] & 0x80808080)
	  overflow();
  }

  void divide(const PackedTerm& a, const PackedTerm& b) {
    ASSERT(a.isDivisibleBy(b));
    sumOfExps = a.sumOfExps - b.sumOfExps;
    for (unsigned int i = 0; i < wordCount; ++i)
      exponents[i] = a.exponents[i] - b.exponents[i];
    SUPPORT(recalculateSupport());
  }


  bool isDivisibleBy(const PackedTerm& term) const {
    if (sumOfExps < term.sumOfExps SUPPORT(|| (term.support & ~support)))
      return false;
    for (unsigned int i = 0; i < wordCount; ++i)
      if ((exponents[i] - term.exponents[i]) & 0x80808080)
	return false;
    return true;
  }

  bool hasDisjunctSupport(const PackedTerm& term) const {
#ifdef PACKED_TERM_NO_SUPPORT
    // hex 7F is binary 0111111, so adding 7F to 0aaaaaaa will give
    // 1bbbbbbb exactly if any a is different from 0. So doing this
    // and and-ing with hex 80 (which is 10000000) tests for zero.
    for (unsigned int i = 0; i < wordCount; ++i)
      if ((exponents[i] + 0x7F7F7F7F) &
	  (term.exponents[i] + 0x7F7F7F7F) &
	  0x80808080)
	return false;
    return true;
#else
    return (support & term.support) == 0;
#endif
  }

  Exponent degree() const {
    return sumOfExps;
  }

  unsigned int max(unsigned int a, unsigned int b) const {return a > b ? a : b;}
  void leastCommonMultiple(const PackedTerm& a, const PackedTerm& b) {
    sumOfExps = 0;
    for (unsigned int i = 0; i < wordCount; ++i) {
      unsigned int ae = a.exponents[i];
      unsigned int be = b.exponents[i];

      unsigned int exponent = 0xFF000000 & max(ae, be);
      sumOfExps += exponent >> 24;
      unsigned int result = exponent;

      exponent = max(ae & 0x00FF0000, be & 0x00FF0000);
      sumOfExps += exponent >> 16;
      result |= exponent;

      exponent = max(ae & 0x0000FF00, be & 0x0000FF00);
      sumOfExps += exponent >> 8;
      result |= exponent;

      exponent = max(ae & 0x000000FF, be & 0x000000FF);
      sumOfExps += exponent;
      result |= exponent;

      exponents[i] = result;
    }
    SUPPORT(recalculateSupport());
  }
  
  const Exponent operator[](int i) const {
    ASSERT(0 <= i && i < (int)Settings::VariableCount);

    // The endianness on an x86 is backwards from what we need, so we
    // have to turn around the value modulo 4. This could probably be
    // done more efficiently by working at the word level and doing
    // bit manipulation, but this function is not called anywhere that
    // needs to be efficient.
    int j = i & 3; // notice that "& 3" is the same as "% 4"
    return (int)((unsigned char*)exponents)[i - j + 3 - j];
  }

  bool operator==(const PackedTerm& term) const {
    if (sumOfExps != term.sumOfExps SUPPORT(|| support != term.support))
      return false;
    for (unsigned int i = 0; i < wordCount; ++i)
      if (exponents[i] != term.exponents[i])
	return false;
    return true;
  }

  bool operator<(const PackedTerm& term) const {
    return compare(*this, term) < 0;
  }

  static int compare(const PackedTerm& a, const PackedTerm& b) {
    int sumDiff = a.sumOfExps - b.sumOfExps;
    if (sumDiff != 0)
      return sumDiff;

#ifndef PACKED_TERM_NO_SUPPORT
    // I think this is a net win, but maybe not. Should do more
    // testing. The point is that we know that the terms are
    // different, so we do not have to test for i < wordCount.
    if (a.support != b.support) {
      for (unsigned int i = 0;; ++i) {
	Exponent ea = a.exponents[i];
	Exponent eb = b.exponents[i];
	if (ea == eb)
	  continue;

	return (int)(eb - ea);
      }
    }
#endif

    for (unsigned int i = 0; i < wordCount; ++i) {
      Exponent ea = a.exponents[i];
      Exponent eb = b.exponents[i];
      if (ea == eb)
	continue;

      // This only works because the high order bit is 0 in both ea
      // and eb. This combines with unsigned overflow/underflow to
      // give a correct signed value.
      return (int)(eb - ea);
    }
    return 0;
  }

  void printDebug(ostream& out) const {
    cout << *this << " has total degree " << sumOfExps <<
      " and data ";
    for (unsigned int i = 0; i < wordCount; ++i)
      cout << hex << exponents[i] << ' ';
    cout << endl << dec;
  }

 private:
#ifndef PACKED_TERM_NO_SUPPORT
  void recalculateSupport() {
    support = 0;
    for (unsigned int i = 0; i < wordCount; ++i) {
      // hex 7F is binary 0111111, so adding 7F to 0aaaaaaa will give
      // 1bbbbbbb exactly if any a is different from 0. So doing this
      // and anding with hex 80 (which is 10000000) tests for zero.

      unsigned int spreadSubSupport = exponents[i] + 0x7F7F7F7F;
      unsigned int subSupport = (spreadSubSupport & 0x80000000);
      subSupport |= (spreadSubSupport & 0x00800000) << 7;
      subSupport |= (spreadSubSupport & 0x00008000) << 14;
      subSupport |= (spreadSubSupport & 0x00000080) << 21;

      // subSupport now contains the support of exponents[i] in
      // the higher 4 bits.
      support |= subSupport >> (i << 2);
    }
  }

  unsigned int support;
#endif

  static void overflow() {
    ::overflow("exponent");
  }

  Exponent exponents[wordCount];
  Exponent sumOfExps;
};

inline ostream& operator<<(ostream& out, const PackedTerm& term) {
  int lastNonZero = Settings::VariableCount - 1;
  for (; lastNonZero >= 0; --lastNonZero)
    if (term[lastNonZero] != 0)
      break;

  if (lastNonZero == -1)
    out << '1';
  else {
    for (int i = 0; i <= lastNonZero; ++i) {
      if (term[i] != 0) {
	out << 'x' << i;
	if (term[i] != 1)
	  out << '^' << term[i];
      }
    }
  }

  return out;
}

#endif
