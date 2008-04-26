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

  class TermConsumer {
  public:
    virtual ~TermConsumer();

    // exponentVector is an array containing the exponents of a term.
    // You may not retain a reference to the array exponentVector itself
    // or to any of the integers pointed to by the entries of exponentVector.
    // You may not alter the array exponentVector or any of the integers
    // pointed to. Doing so will result in undefined behavior.
    virtual void consume(mpz_ptr* exponentVector) = 0;
  };

  // Compute the Alexander dual of ideal using the point
  // exponentVector. The minimal generators of the dual are provided to
  // the consumer. If exponentVector is null, then the lcm of the
  // minimal generators of ideal are used. Note that this lcm does not
  // include any non-minimal generators that may have been added to ideal.
  //
  // The parameter exponentVector must be either null or an array of size at
  // least the number of variables of ideal. Frobby does not alter or retain
  // a reference to the passed-in exponentVector or its elements.
  // This is also true when the consumer gets called, so it is allowed
  // for the consumer to change or delete exponentVector.
  void alexanderDual(const Ideal& ideal,
		     const mpz_t* exponentVector,
		     TermConsumer& consumer);
}

#endif
