#ifndef SLICE_ALGORITHM_GUARD
#define SLICE_ALGORITHM_GUARD

#include "Term.h"
#include "VarNames.h"
#include <ostream>

class Ideal;
class Term;
class IOHandler;
class TermTranslator;
class Partition;

class SliceAlgorithm {
 public:
  SliceAlgorithm(const Ideal& ideal,
		 const VarNames& names,
		 const TermTranslator* translator,
		 ostream& out);

  class DecomConsumer {
  public:
    virtual ~DecomConsumer() {}
    
    virtual void consume(const Term& term) = 0;
  };

 private:
  class Slice {
  public:
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

    bool baseCase(DecomConsumer* consumer) const;

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
    const size_t _varCount;
    Term _multiply;
    mutable Term _lcm;

    Ideal* _ideal;
    Ideal* _subtract;
  };

  void content(Slice& slice);
  void labelSplit(Slice& slice);
  void pivotSplit(Slice& slice);
  bool independenceSplit(Slice& slice);

  struct ProjDecom {
    vector<Exponent> compressor;
    Ideal* decom;
  };
  void inverseDecomProject(const vector<ProjDecom>& projs,
			   Partition& partition,
			   size_t proj,
			   Term& term);

  void getPivot(const Slice& slice, Term& pivot);

  DecomConsumer* _decomConsumer;
};

#endif
