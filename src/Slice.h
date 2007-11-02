#ifndef SLICE_GUARD
#define SLICE_GUARD

#include "Term.h"

class Ideal;
class DecomConsumer;

class Slice {
 public:
  void clear() {
    _subtract = 0;
    _ideal = 0;
  }

  Slice(Ideal* ideal, Ideal* subtract,
	size_t varCount);
  Slice(Ideal* ideal, Ideal* subtract,
	const Term& multiply);
  Slice(Ideal* ideal, Ideal* subtract,
	const Term& multiply, const Term& pivot);
  ~Slice();

  void addToSubtract(const Term& term);
  bool normalize();
  bool filterDoubleLcm();
  void simplify();

  bool baseCase(DecomConsumer* consumer);

  const Term& getLcm() const;

  Ideal* getIdeal() {return _ideal;}
  const Ideal* getIdeal() const {return _ideal;}

  Ideal* getSubtract() {return _subtract;}
  const Ideal* getSubtract() const {return _subtract;}

  Term& getMultiply() {return _multiply;}
  const Term& getMultiply() const {return _multiply;}

  size_t getVarCount() const {return _varCount;}

  void print() const;

 private:
  bool twoVarBaseCase(DecomConsumer* consumer);

  const size_t _varCount;
  Term _multiply;
  mutable Term _lcm;

  Ideal* _ideal;
  Ideal* _subtract;
};

#endif
