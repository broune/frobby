#ifndef IDEAL_MINIMIZER_GUARD
#define IDEAL_MINIMIZER_GUARD

#include <vector>

class Term;

class Minimizer {
 private:
  typedef vector<Exponent*>::iterator iterator;

 public:
  Minimizer(size_t varCount):
	_varCount(varCount) {}

  iterator minimize(iterator begin, iterator end) const;

  iterator colonReminimize(iterator begin, iterator end, const Term& colon);
  iterator colonReminimize(iterator begin, iterator end,
						   size_t var, Exponent exponent);

  bool dominatesAny(iterator begin, iterator end, const Exponent* term);
  bool dividesAny(iterator begin, iterator end, const Exponent* term);

 private:
  iterator colonReminimizePreprocess(iterator beginm, iterator end,
									 const Term& colon);
  bool simpleIsMinimal(iterator begin, iterator end);

  size_t _varCount;
};

#endif
