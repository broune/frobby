#include "../stdinc.h"
#include "LabelAlgorithm.h"

#include "Strategy.h"
#include "TermTree.h"
#include "OldPartition.h"

#include "../Term.h"

LabelAlgorithm::LabelAlgorithm(Strategy* strategy,
			       TermTree* tree,
			       bool usePartition):
  _dimension(tree->getDimension()),
  _strategy(strategy),
  _usePartition(usePartition) {
  ASSERT(_dimension > 1);
  
  Term b(_dimension);
  recurse(b, *tree, false);
}

bool LabelAlgorithm::
performPartition(const Term& b, const TermTree& tree) {
  Term term(_dimension);
  TermTree::TreeIterator it(tree);

  if (tree.getPosition() > (int)_dimension - 6)
    return false;
    
  unsigned int size = 0;

  OldPartition partition(_dimension);
  while (!it.atEnd()) {
    it.getTerm(term);
    ++size;
    int first = -1;
    for (unsigned int i = tree.getPosition(); i < _dimension; ++i) {
      if (term[i] > 0) {
	if (first == -1)
	  first = i;
	else
	  partition.join(first, i);
      }
    }
    ++it;
  }
    
  if (size < 15)
    return false;
    
  ASSERT(partition.getSetCount(1, tree.getPosition()) > 0);

  if (partition.getSetCount(1, tree.getPosition()) <= 1)
    return false;

  vector<Exponent> compressor;
  vector<Exponent> decompressor;

  _strategy->startingPartitioning(b, partition, tree);

  for (int i = 0; i < partition.getSetCount(1, tree.getPosition()); ++i) {
    partition.getSetTranslators(i, compressor, tree.getPosition());

    Term newB(b);
    partition.compress(newB, compressor);

    TermTree nextTree(newB, _dimension, _dimension - partition.getSetSize(i, tree.getPosition()));
    TermTree::TreeIterator treeIt(tree);
    while (!treeIt.atEnd()) {
      treeIt.getTerm(term);
	
      if (partition.compress(term, compressor))
	nextTree.insert(term);
      ++treeIt;
    }

    _strategy->doingPartitionSet(tree.getPosition(), b, newB, partition, compressor, nextTree);
    /*
      cout << "--------------" << endl;
      cout << "total dimension: " << _dimension << endl;
      cout << "setSize: " << partition.getSetSize(i, tree.getPosition()) << endl;
      cout << "compressor " << i << ":";
      for (unsigned int j = 0; j < compressor.size(); ++j)
      cout << ' ' << compressor[j];
      cout << endl;
      tree.print(cout);
      nextTree.print(cout);
    //*/
    if (nextTree.getPosition() == (int)_dimension - 1) {
      TermTree::TreeIterator tmpIt(nextTree);
      newB[_dimension - 1] = 	tmpIt.getExponent(_dimension - 1) - 1;

      _strategy->foundSolution(newB, true);
    } else
      recurse(newB, nextTree, true);
      
    _strategy->doneDoingPartitionSet(tree.getPosition(), compressor);
  }
  _strategy->endingPartitioning(tree.getPosition(), b);

  return true;
}

void LabelAlgorithm::
recurse(const Term& b, const TermTree& tree, bool startingPartition) {
  if (tree.empty()) {
    // This only happens if the input ideal was not minimized.
    // TODO: report this as an error ONCE somehow.
    return;
  }

  unsigned int position = tree.getPosition();

  if (!_strategy->startingCall(b, tree, startingPartition))
    return;
   
  if (position == _dimension - 2) {
    baseCase(b, tree);
    _strategy->endingCall(b, tree);
    return;
  }

  if (_usePartition && performPartition(b, tree))
    return;

  Term newB(b);

  TermTree::TreeIterator treeIt(tree);
  Term tmpTerm(_dimension);
  while (!treeIt.atEnd()) {
    treeIt.getTerm(tmpTerm);
    if (tmpTerm[position] > b[position])
      break;
    ++treeIt;
  }
  Exponent lastExponent = 0;

  TermTree nextTree(b, _dimension, position + 1);
  nextTree.insertProjectionOf(tree, 0, 0);

  while (!treeIt.atEnd()) {
    treeIt.getTerm(tmpTerm);

    // Does not change entries prior to position, so we can keep
    // using newB in later iterations without resetting it to b.
    newB.lcm(b, tmpTerm, position);
    newB[position] = tmpTerm[position] - 1;
      
    ++treeIt;
    bool sameExponentAsNext = false;
    if (!treeIt.atEnd() &&
	tmpTerm[position] == treeIt.getExponent(position))
      sameExponentAsNext = true;

    if (!_strategy->consideringCall(newB, sameExponentAsNext, tree))
      continue;

    nextTree.insertProjectionOf(tree, lastExponent + 1, newB[position]);
    lastExponent = newB[position];

    if (!(newB == b)) {
      TermTree nextTreeOpt(newB, _dimension, position + 1);
      nextTreeOpt.insert(nextTree);

      recurse(newB, nextTreeOpt, false);
    } else {
      recurse(newB, nextTree, false);
    }
  }

  _strategy->endingCall(b, tree);
}

void LabelAlgorithm::baseCase(const Term& b, const TermTree& tree) {
  ASSERT(!tree.empty());

  size_t position = tree.getPosition();

  TermTree::TreeIterator treeIt(tree);
  TermTree::TreeIterator treeNextIt(tree);
  ++treeNextIt;

  Term newB(b);
  while (!treeNextIt.atEnd()) {
    ASSERT(treeNextIt.getExponent(position) > 0);
    ASSERT(treeIt.getExponent(position + 1) > 0);

    newB[position] = treeNextIt.getExponent(position) - 1;
    newB[position + 1] = treeIt.getExponent(position + 1) - 1;

    _strategy->foundSolution(newB, false);
	
    ++treeIt;
    ++treeNextIt;
  }
}
