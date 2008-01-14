#ifndef IDEAL_MINIMIZER_GUARD
#define IDEAL_MINIMIZER_GUARD

#include <vector>

class Minimizer {
 private:
  typedef vector<Exponent*>::iterator iterator;

 public:
  Minimizer(size_t varCount):
	_varCount(varCount) {}

  iterator minimize(iterator begin, iterator end) const;

 private:
  size_t _varCount;
};

#endif
