#ifndef SLICE_STRATEGY_GUARD
#define SLICE_STRATEGY_GUARD

#include <string>

class Slice;
class Term;

class SliceStrategy {
 public:
  virtual ~SliceStrategy();

  enum SplitType {
    LabelSplit = 1,
    PivotSplit = 2
  };

  virtual void startingContent(const Slice& slice);
  virtual void endingContent();

  virtual SplitType getSplitType(const Slice& slice) = 0;

  virtual void getPivot(Term& pivot, const Slice& slice);
  virtual size_t getLabelSplitVariable(const Slice& slice);

  static SliceStrategy* newStrategy(const string& name);
  static SliceStrategy* addStatistics(SliceStrategy* strategy);
  static SliceStrategy* addDebugOutput(SliceStrategy* strategy);
};

#endif
