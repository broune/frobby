#ifndef STATISTICS_STRATEGY_GUARD
#define STATISTICS_STRATEGY_GUARD

#include "Strategy.h"
#include "TermTree.h"
#include "TermTranslator.h"
#include "VarNames.h"
#include "Partition.h"

class StatisticsStrategy : public Strategy {
public:
  StatisticsStrategy(int dimension):
    _dimension(dimension),
    _callCounts(dimension) {
  }

  virtual ~StatisticsStrategy() {
    for (unsigned int position = 0; position < _dimension; ++position)
      cerr << "Level " << position + 1 << " had "
	   << _callCounts[position] << " calls. " << endl;
  }

  virtual void getName(string& name) const {
    name = "StatisticsStrategy";
  }

  virtual bool consideringCall(const ExternalTerm& b,
			       bool sameExponentAsNext,
			       const TermTree& tree) {
    return false;
  }

  virtual bool startingCall(const ExternalTerm& b,
			    const TermTree& tree,
			    bool startingPartition) {
    return false;
  }

  virtual void endingCall(const ExternalTerm& b,
			  const TermTree& tree) {
    ++(_callCounts[tree.getPosition()]);
  }

  virtual void foundSolution(const ExternalTerm& b,
			     bool startingPartition) {
    ++(_callCounts[_dimension - 1]);
  }

  virtual void startingPartitioning(const ExternalTerm& b,
				    const Partition& partition,
				    const TermTree& tree) {
  }
    
  virtual void doingPartitionSet(int position,
				 const ExternalTerm& b,
				 const ExternalTerm& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  }

  virtual void doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  }

  virtual void endingPartitioning(int position,
				  const ExternalTerm& b) {
  }

private:
  unsigned int _dimension;
  vector<unsigned int> _callCounts;
};

#endif
