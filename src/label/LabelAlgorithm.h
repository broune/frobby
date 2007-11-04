#ifndef LABEL_ALGORITHM_GUARD
#define LABEL_ALGORITHM_GUARD

class Term;
class Strategy;
class TermTree;

class LabelAlgorithm {
public:
  LabelAlgorithm();

  void setStrategy(Strategy* strategy);
  void setUsePartition(bool usePartition);

  void runAndDeleteIdealAndReset(TermTree* tree);

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
