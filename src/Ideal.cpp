#include "stdinc.h"
#include "Ideal.h"

#include "Term.h"

Ideal::~Ideal() {
}

bool Ideal::isIrreducible() const {
  size_t varCount = getVarCount();
  const_iterator stop = end();
  for (const_iterator it = begin(); it != stop; ++it)
    if (getSizeOfSupport(*it, varCount) != 1)
      return false;
  return true;
}
