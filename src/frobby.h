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
#ifndef FROBBY_INCLUSION_GUARD
#define FROBBY_INCLUSION_GUARD

// This file contains the public library interface of Frobby.

#include "gmp.h"

// The namespace FrobbyImpl is for internal use inside Frobby only.
// It is not part of the public interface of Frobby.
namespace FrobbyImpl {
  class FrobbyIdealHelper;
}

// The namespace Frobby contains the public interface of Frobby.
namespace Frobby {
  class Ideal {
  public:
    // Construct an ideal with variableCount variables.
    Ideal(size_t variableCount);
    Ideal(const Ideal& ideal);
    ~Ideal();

    Ideal& operator=(const Ideal& ideal);

    // Call addExponent once for each variable to add a term one exponent at
    // a time. Ideal keeps track of how many times you called
    // addExponent to know which variable is to be raised to the passed-in
    // exponent. Frobby does not alter the passed-in exponent and retains no
    // reference to it. Exponent must be non-negative. It is allowed to
    // mix the different overloads even within the same term.
    //
    // If the number of variables is zero, then a call to addExponent adds
    // the element 1 as a generator. In that case it makes no difference
    // what the value of exponent is.
    void addExponent(const mpz_t exponent);
    void addExponent(int exponent);
    void addExponent(unsigned int exponent);

  private:
    friend class FrobbyImpl::FrobbyIdealHelper;
    FrobbyImpl::FrobbyIdealHelper* _data;
  };

  // The consumer classes below provide a way to get output from Frobby
  // one term at a time. Most computations performed by Frobby can
  // produce output continously instead of having to wait for the
  // computation to be done to be able to produce any output. Often
  // the output can be processed, such as writing it to disk, as it is
  // produced, and in these cases there is no reason to store all of
  // the output before processing any of it. Often just storing the
  // output can consume many times the amount of memory Frobby
  // otherwise needs to carry out the computation.
  //
  // The parameters passed to the consume method of a consumer must
  // not be altered or deallocated, and no reference to them can be
  // retained after the consume method has returned. If the output
  // must be stored, then a copy must be made.

  // For output of monomial ideals from Frobby.
  class IdealConsumer {
  public:
	// The provided implementation does nothing.
    virtual ~IdealConsumer();

	// Called before output of a monomial ideal. varCount is the
	// number of variables of the output ideal. The provided
	// implementation does nothing.
	virtual void idealBegin(size_t varCount);

	// For output of a generator of the ideal. exponentVector is an
	// array containing the exponents of the generator.
    virtual void consume(mpz_ptr* exponentVector) = 0;

	// Called after output of a monomial ideal. The provided
	// implementation does nothing.
	virtual void idealEnd();
  };

  // This typedef is for backwards compatability. The use of the name
  // TermConsumer is deprecated and will be removed in a future
  // version of Frobby.
  typedef IdealConsumer TermConsumer;

  // For output of polynomials from Frobby.
  class PolynomialConsumer {
  public:
	// The provided implementation does nothing.
	virtual ~PolynomialConsumer();

	// Called before output of a polynomial. varCount is the number of
	// variables of the output polynomial. The provided implementation
	// does nothing.
	virtual void polynomialBegin(size_t varCount);

	// For output of a term of the polynomial. coefficient contains
	// the coefficient of the term, and exponentVector is an array
	// containing the exponents of the term.
	virtual void consume(const mpz_t coefficient, mpz_ptr* exponentVector) = 0;

	// Called after output of a polynomial. The provided
	// implementation does nothing.
	virtual void polynomialEnd();
  };

  // Compute the Alexander dual of ideal using the point
  // exponentVector. The minimal generators of the dual are provided
  // to the consumer in some arbitrary order. If exponentVector is
  // null, then the lcm of the minimal generators of ideal are
  // used. Note that this lcm does not include any non-minimal
  // generators that may have been added to ideal.
  //
  // The parameter exponentVector must be either null or an array of
  // size at least the number of variables of ideal. Frobby does not
  // alter or retain a reference to the passed-in exponentVector or
  // its elements.  This is also true when the consumer gets called,
  // so it is allowed for the consumer to change or delete
  // exponentVector.
  void alexanderDual(const Ideal& ideal,
                     const mpz_t* exponentVector,
                     IdealConsumer& consumer);

  // Compute the multigraded Hilbert-Poincare series of ideal. More
  // precisely, compute the numerator polynomial of the series
  // expressed as a rational function with (1-x1)...(1-xn) in the
  // denominator where x1,...,xn are the variables in the polynomial
  // ring. The multigraded Hilbert-Poincare series of a monomial ideal
  // is the possibly infinite sum of all monomials not in that ideal.
  void multigradedHilbertPoincareSeries(const Ideal& ideal,
										PolynomialConsumer& consumer);

  // Compute the univariate Hilbert-Poincare series of ideal. The
  // univariate series can be obtained from the multigraded one by
  // substituting the same variable for each other variable.
  void univariateHilbertPoincareSeries(const Ideal& ideal,
									   PolynomialConsumer& consumer);

  // Compute the irreducible decomposition of ideal. Every monomial
  // ideal can be written uniquely as the irredundant intersection of
  // irreducible monomial ideals, and each intersectand in this
  // intersection is called a irreducible component. A monomial ideal
  // is irreducible if and only if each minimal generator is a pure
  // power, i.e. has the form x^e for a variable x and an integer
  // exponent e.
  //
  // The output is each of the irreducible components. These are
  // provided to the consumer in some arbitrary order. The ideal
  // generated by the identity has no irreducible components, since
  // the intersection of no ideals is the entire ring, i.e. the ideal
  // generated by the identity. The output for the zero ideal is a
  // single ideal which is the zero ideal itself.
  void irreducibleDecompositionAsIdeals(const Ideal& ideal,
										IdealConsumer& consumer);

  // irreducibleDecompositionAsMonomials computes the irreducible
  // decomposition of ideal, and encodes each irreducible component as
  // a monomial. This is done by multiplyng the minimal generators of
  // the irreducible ideal, which defines a bijection between
  // irreducible monomial ideals and monomials.
  //
  // Note that the ideal generated by the identity has no irreducible
  // components, so the output is the zero ideal, since that ideal has
  // no generators.
  //
  // The zero ideal as input presents a problem, since no monomial
  // corresponds to the zero ideal. If ideal is the zero ideal, then
  // there is no output (i.e. idealBegin does not get called on the
  // consumer), and the return value is false. Otherwise the return
  // value is true.
  bool irreducibleDecompositionAsMonomials(const Ideal& ideal,
										   IdealConsumer& consumer);

  // Compute the maximal standard monomials. A standard monomial of an
  // ideal is a monomial that does not lie in that ideal. A maximal
  // standard monomial is a standard monomial m such that mx is not
  // standard for each variable x in the polynomial ring within which
  // the ideal resides.
  //
  // Note that some ideals have no maximal standard
  // monomials. E.g. every monomial is a standard monomial of the zero
  // ideal, so none of them are maximal. In contrast, the ideal
  // generated by the identity has no standard monomials, maximal or
  // otherwise.
  //
  // The maximal standard monomials are output to the consumer as the
  // generators of a single ideal. If ideal has no maximal standard
  // monomials, then the output is the zero ideal.
  void maximalStandardMonomials(const Ideal& ideal,
								IdealConsumer& consumer);

  // Solve the optimization program
  //
  //    maximize l*d subject to d being the exponent vector of a
  //    maximal standard monomial of ideal
  //
  // If ideal has any maximal standard monomials, then the return
  // value is true, and an ideal with a single generator representing
  // an optimal solution to the optimization program is written to the
  // consumer. If ideal has no maximal standard monomials, then the
  // return value is false, and nothing is written to the consumer.
  //
  // l must be an array of at least ideal.getVarCount() entries.
  //
  // The algorithm used is significantly faster than computing all the
  // maximal standard monomials and seeing which exponent vector
  // yields a maximum value. The current implementation has the
  // limitation that this only works if the entries of l are
  // non-negative. Frobby will still compute the correct value if some
  // entry of l is strictly negative, but it will simply fall back to
  // computing all the maximal standard monomials. This is a
  // limitation of this particular implementation rather than the
  // algorithm, and the implementation can be extended to the case of
  // negative entries without too much trouble. Feel free to contact
  // the author of Frobby if you have a use for this.
  bool solveStandardMonomialProgram(const Ideal& ideal,
									const mpz_t* l,
									IdealConsumer& consumer);
}

#endif
