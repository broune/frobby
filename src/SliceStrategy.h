#ifndef SLICE_STRATEGY_GUARD
#define SLICE_STRATEGY_GUARD

#include <string>
#include "DecomConsumer.h"

class Slice;
class Term;
class TermTranslator;

class SliceStrategy : public DecomConsumer {
 public:
  virtual ~SliceStrategy();

  enum SplitType {
    LabelSplit = 1,
    PivotSplit = 2
  };

  virtual void startingContent(const Slice& slice);
  virtual void endingContent();

  virtual void simplify(Slice& slice);

  virtual SplitType getSplitType(const Slice& slice) = 0;

  virtual void getPivot(Term& pivot, const Slice& slice);
  virtual size_t getLabelSplitVariable(const Slice& slice);

  virtual void consume(const Term& term) = 0;


  static SliceStrategy* newDecomStrategy(const string& name,
					 DecomConsumer* consumer);

  static SliceStrategy* newFrobeniusStrategy(const string& name,
					     const vector<mpz_class>& instance,
					     const TermTranslator* translator,
					     mpz_class& frobeniusNumber);

  static SliceStrategy* addStatistics(SliceStrategy* strategy);
  static SliceStrategy* addDebugOutput(SliceStrategy* strategy);
};

#endif
