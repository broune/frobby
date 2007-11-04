#ifndef PROJECTION_GUARD
#define PROJECTION_GUARD

#include <vector>

class Partition;
class Term;

class Projection {
 public:
  Projection() {}

  void reset(const Partition& partition, int set);
  size_t getRangeVarCount() const;

  void project(Term& to, const Exponent* from) const;
  void inverseProject(Term& to, const Exponent* from) const;
  
 private:
  vector<size_t> _offsets;
};

#endif
