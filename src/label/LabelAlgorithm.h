#ifndef LABEL_ALGORITHM_GUARD
#define LABEL_ALGORITHM_GUARD

class Term;
class Strategy;
class TermTree;
class Ideal;

class LabelAlgorithm {
public:
  LabelAlgorithm();

  void setStrategy(Strategy* strategy);
  void setUsePartition(bool usePartition);

  // Runs the algorithm and clears ideal. Then deletes the strategy.
  void runAndClear(Ideal& ideal);

private:
  bool performPartition(const Term& b, const TermTree& tree);
  void recurse(const Term& b, const TermTree& tree,
	       bool startingPartition);
  void baseCase(const Term& b, const TermTree& tree);

  unsigned int _dimension;
  Strategy* _strategy;
  bool _usePartition;
};

#endif
