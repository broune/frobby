#ifndef UWE_GUARD
#define UWE_GUARD

#include <vector>
class BigIdeal;

void computePrimaryDecom(const BigIdeal& ideal, vector<BigIdeal*>& ideals);
void computeAssociatedPrimes(const BigIdeal& ideal, BigIdeal& primes);

#endif
