#ifndef FROBBY_GUARD
#define FROBBY_GUARD

#include "gmp.h"

namespace Frobby {
  class ExternalIdeal {
  public:
	ExternalIdeal(size_t variableCount, size_t expectedGeneratorCount);
	ExternalIdeal(size_t variableCount);
	~ExternalIdeal();

	// Add a generator to the ideal. The parameter exponentVector must
	// be an array of size at least the number of variables.
	void addGenerator(const mpz_ptr* exponentVector);
	void addGenerator(const mpz_t* exponentVector);

	// This method is for internal use inside Frobby only. It is not
	// part of the public interface.
	void* getData();

  private:
	void* _data;
  };

  class ExternalTermConsumer {
  public:
	virtual ~ExternalTermConsumer();
	virtual void consume(mpz_ptr* exponentVector) = 0;
  };

  // Compute the Alexander dual of ideal using the point
  // exponentVector. The parameter exponentVector must be an array of
  // size at least the number of variables. The minimal generators of
  // the dual are provided to the consumer.
  void alexanderDual(ExternalIdeal* ideal, const mpz_ptr* exponentVector,
					 ExternalTermConsumer* consumer);
}

#endif
