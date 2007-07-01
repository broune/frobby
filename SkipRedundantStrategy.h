#ifndef SKIP_REDUNDANT_STRATEGY_GUARD
#define SKIP_REDUNDANT_STRATEGY_GUARD

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

class SkipRedundantStrategy : public Strategy {
public:
  SkipRedundantStrategy(Strategy* strategy,
			unsigned int dimension):
    _newSkip(dimension),
    _remember(dimension),
    _dimension(dimension),
    _strategy(strategy),
    _toAddNext(dimension),
    _seenHasChanged(dimension) {
    for (unsigned int position = 0; position < _dimension; ++position)
      _skip.push_back(TermTree(dimension, position));
  }

  virtual ~SkipRedundantStrategy() {}

  virtual void getName(string& name) const {
    name = "SkipRedundantStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    unsigned int position = tree.getPosition();

    _remember[position] = sameExponentAsNext;

    //cout << "looking at possible skip for " << position << ' ' << b << endl;
    //print(cout);

    if (_skip[position].getDivisor(b) != 0) {
      if (!sameExponentAsNext)
	_newSkip[position + 1].clear();
      //cout << "skipped" << endl;
      return false;
    }

    return _strategy->consideringCall(b, sameExponentAsNext, tree);
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    unsigned int position = tree.getPosition();

    if (!startingPartition) {
      _skip[position].clear();
      _skip[position].setThreshold(b);
      if (position > 0)
	_skip[position].insertProjectionOf(_skip[position - 1], 0, b[position - 1]);
      for (unsigned int i = 0; i < _newSkip[position].size(); ++i)
	_skip[position].insert(_newSkip[position][i]);
    }

    _remember[position] = false;
      
    return _strategy->startingCall(b, tree, startingPartition);
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    unsigned int position = tree.getPosition();

    if (position > 0) {
      if (_remember[position - 1])
	_newSkip[position].push_back(b);
      else
	_newSkip[position].clear();
    }

    _strategy->endingCall(b, tree);
  }
  
  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    //cout << "sol " << b << ' ' << startingPartition << endl;
    //print(cout);
    if ((startingPartition && _skip[_dimension - 1].getDivisor(b) == 0) ||
	(!startingPartition && _skip[_dimension - 2].getDivisor(b) == 0))
      _strategy->foundSolution(b, startingPartition);
    //else
    //  cout << "skipped sol" << endl;
  }


  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
    unsigned int position = tree.getPosition();

    _skip[position].clear();
    _skip[position].setThreshold(b);
    if (position > 0)
      _skip[position].insertProjectionOf(_skip[position - 1], 0, b[position - 1]);
    for (unsigned int i = 0; i < _newSkip[position].size(); ++i)
      _skip[position].insert(_newSkip[position][i]);

    _remember[position] = false; // TODO: is this necessary?

    _strategy->startingPartitioning(b, partition, tree);
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
    int nextPosition = tree.getPosition();
    TermTree::TreeIterator it((TermTree&)_skip[position]);

    _skip[nextPosition].clear();
    _skip[nextPosition].setThreshold(compressedB);

    //cout << "compressing _skip. b=" << b << ", nextPosition=" << nextPosition << endl;
    ExternalTerm term(_dimension);
    while (!it.atEnd()) {
      it.getTerm(term);

      //cout << "compressing: " << term << endl;

      for (int i = _dimension - 1; i >= position; --i) {
	if (compressor[i] == 0xFFFFFFFF) {
	  if (term[i] > b[i])
	    goto skip;
	} else
	  term[compressor[i]] = term[i];
      }
      _skip[nextPosition].insert(term);

    skip:
      ++it;
    }

    _strategy->doingPartitionSet(position, b, compressedB, partition, compressor, tree);
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
    _strategy->doneDoingPartitionSet(position, compressor);
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
    if (position > 0) {
      if (_remember[position - 1])
	_newSkip[position].push_back(b);
      else
	_newSkip[position].clear();
    }

    _strategy->endingPartitioning(position, b);
  }

  void print(ostream& out) {
    // If there is a segmentation violation below, the flush will make
    // it clear that this is where it happened.
    out << "SkipRedundantStrategy(dimension=" << flush;
    out << _dimension << '\n';
    out << " remember=";
    copy(_remember.begin(), _remember.end(),
	 ostream_iterator<bool>(out));
    out << '\n';

    out << " skip=\n";
    for (unsigned int i = 0; i < _dimension; ++i) {
      out << "  " << i << ": ";
      _skip[i].print(out);
    }

    out << " newSkip=\n";
    for (unsigned int i = 0; i < _dimension; ++i) {
      out << "  " << i << ": ";
      copy(_newSkip[i].begin(), _newSkip[i].end(),
	   ostream_iterator<ExternalTerm>(out));
      out << '\n';
    }

    out << ")" << endl;
  }

private:
  vector<TermTree> _skip;
  vector<vector<ExternalTerm> > _newSkip;
  vector<bool> _remember;
  unsigned int _dimension;
  Strategy* _strategy;

  vector<Exponent> _toAddNext;
  vector<TermTree> _seen;
  vector<TermTree> _projectedSeen;
  vector<bool> _seenHasChanged;
};

#endif
