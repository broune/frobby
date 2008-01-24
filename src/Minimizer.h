#ifndef IDEAL_MINIMIZER_GUARD
#define IDEAL_MINIMIZER_GUARD

#include <vector>

class Minimizer {
 private:
  typedef vector<Exponent*>::iterator iterator;
  typedef vector<Exponent*>::const_iterator const_iterator;

 public:
  Minimizer(size_t varCount):
	_varCount(varCount) {}

  iterator minimize(iterator begin, iterator end) const;

  pair<iterator, bool> colonReminimize(iterator begin, iterator end,
									   const Exponent* colon);
  pair<iterator, bool> colonReminimize(iterator begin, iterator end,
									   size_t var, Exponent exponent);

  bool dominatesAny(iterator begin, iterator end, const Exponent* term);
  bool dividesAny(iterator begin, iterator end, const Exponent* term);

  bool isMinimallyGenerated(const_iterator begin, const_iterator end);

 private:
  size_t _varCount;
};

#endif
