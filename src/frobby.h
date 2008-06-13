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

  // This class is a way for you to get output from Frobby one term at
  // a time. Most computations performed by Frobby can produce output
  // continously instead of having to wait for the computation to be
  // done to be able to produce any output. Often the output can be
  // processed, such as writing it to disk, as it is produced, and in
  // these cases there is no reason to store all of the output before
  // processing any of it. Often just storing the output can consume
  // many times the amount of memory Frobby otherwise needs to carry
  // out the computation.
  class TermConsumer {
  public:
    virtual ~TermConsumer();

	// This method is used to output a term with no associated
	// coefficient. Either this method is used to produce all of the
	// output from a computation, or it is not used at all, i.e. this
	// method is not used in case the output has a coefficient of 1.
	//
    // exponentVector is an array containing the exponents of a term.
    // You may not retain a reference to the array exponentVector itself
    // or to any of the integers pointed to by the entries of exponentVector.
    // You may not alter the array exponentVector or any of the integers
    // pointed to. Doing so will result in undefined behavior.
	//
	// The provided implementation of this method does nothing.
    virtual void consume(mpz_ptr* exponentVector);

    // This method is used to ouput a term with an associated
	// coefficient. The restrictions on use of exponentVector are the
	// same as for the other overload. You may not alter or deallocate
	// the integer in the paramter coefficient.
	//
	// The provided implemenation of this method does nothing.
	virtual void consume(const mpz_t coefficient, mpz_ptr* exponentVector);
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
                     TermConsumer& consumer);

  // Compute the muligraded Hilbert-Poincare series of ideal. More
  // precisely, compute the numerator polynomial of the series
  // expressed as a rational function with (1-x1)...(1-xn) in the
  // denominator where x1,...,xn are the variables in the polynomial
  // ring. The multigraded Hilbert-Poincare series of a monomial ideal
  // is the possibly infinite sum of all monomials not in that ideal.
  void multigradedHilbertPoincareSeries(const Ideal& ideal,
										TermConsumer& consumer);

  // Compute the univariate Hilbert-Poincare series of ideal. The
  // univariate series can be obtained from the multigraded one by
  // substituting the same variable for each other variable.
  void univariateHilbertPoincareSeries(const Ideal& ideal,
									   TermConsumer& consumer);
}

#endif
