#include "../stdinc.h"
#include "PrintDebugStrategy.h"

#include "TermTree.h"

#include <iterator>

PrintDebugStrategy::~PrintDebugStrategy() {
  fputs("The computation is done.\n", stderr);
}

void PrintDebugStrategy::getName(string& name) const {
  name = "PrintDebugStrategy";
}

bool PrintDebugStrategy::consideringCall(const Term& b,
					 bool sameExponentAsNext,
					 const TermTree& tree) {
  return false;
}

bool PrintDebugStrategy::startingCall(const Term& b,
				      const TermTree& tree,
				      bool startingPartition) {
  if (tree.getPosition() == 0) {
    fputs("Debug printing on. Starting the computation.\n", stderr);
  }
  
  for (int i = 0; i < tree.getPosition(); ++i)
    fputc('^', stderr);
  fputs(" Starting b=", stderr);
  b.print(stderr);
  fprintf(stderr, " position=%lu\n", (unsigned long)tree.getPosition());

  return false;
}

void PrintDebugStrategy::endingCall(const Term& b,
				    const TermTree& tree) {
  for (int i = 0; i < tree.getPosition(); ++i)
    fputc('v', stderr);
  fputs(" Ending b=", stderr);
  b.print(stderr);
  fprintf(stderr, " position=%lu\n", (unsigned long)tree.getPosition());
}

void PrintDebugStrategy::foundSolution(const Term& b,
				       bool startingPartition) {
  for (unsigned int i = 0; i < b.getVarCount(); ++i)
    fputc('*', stderr);
  fputs(" Found solution ", stderr);
  b.print(stderr);
  fputc('\n', stderr);
}

void PrintDebugStrategy::startingPartitioning(const Term& b,
					      const Partition& partition,
					      const TermTree& tree) {
  for (int i = 0; i < tree.getPosition(); ++i)
    fputc('-', stderr);
  fputs(" starting partitioning\n", stderr);
}

void PrintDebugStrategy::doingPartitionSet(int position,
					   const Term& b,
					   const Term& compressedB,
					   const Partition& partition,
					   vector<Exponent> compressor,
					   const TermTree& tree) {
  fputs("doing a set of the partition\n", stderr);
}

void PrintDebugStrategy::doneDoingPartitionSet
(int position,
 const vector<Exponent>& compressor) {
  fputs("done doing that set of the partition\n", stderr);
}

void PrintDebugStrategy::endingPartitioning(int position,
					    const Term& b) {
  fputs("ending partitioning\n", stderr);
}
