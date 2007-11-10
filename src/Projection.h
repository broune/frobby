#ifndef PROJECTION_GUARD
#define PROJECTION_GUARD

#include <vector>

class Partition;
class Term;

// Recall that a function maps elements from its domain to its
// range. Projection represents a function that maps variables from
// its range ring to its domain ring, where some variables can be
// mapped to zero.
class Projection {
 public:
  Projection() {}

  void reset(const Partition& partition, int set);
  size_t getRangeVarCount() const;

  size_t getDomainVar(size_t rangeVar);

  void project(Exponent* to, const Exponent* from) const;

  void inverseProject(Term& to, const Exponent* from) const;

  void print() const;
  
 private:
  vector<size_t> _offsets;
};

#endif
