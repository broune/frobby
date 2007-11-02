#ifndef LABEL_ALGORITHM_GUARD
#define LABEL_ALGORITHM_GUARD

class Term;
class Strategy;
class TermTree;

class LabelAlgorithm {
public:
  LabelAlgorithm(Strategy* strategy, TermTree* tree, bool usePartition);

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
