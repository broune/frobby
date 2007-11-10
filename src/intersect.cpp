#include "stdinc.h"
#include "intersect.h"

#include "Ideal.h"
#include "Term.h"

void intersect(Ideal* output, const Ideal* a, const Ideal* b) {
  ASSERT(a->getVarCount() == b->getVarCount());
  output->clear();

  // TODO: this can be done better. Especially, this method requires
  // too much memory.

  Term term(a->getVarCount());
  for (Ideal::const_iterator aIt = a->begin(); aIt != a->end(); ++aIt) {
    for (Ideal::const_iterator bIt = b->begin(); bIt != b->end();++bIt) {
      term.lcm(*aIt, *bIt);
      output->insert(term);
    }
  }

  output->minimize();
}
