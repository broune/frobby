#include "stdinc.h"
#include "IndependenceSplitter.h"

IndependenceSplitter::IndependenceSplitter(Slice& slice):
  _slice(slice) {}

bool IndependenceSplitter::shouldPerformSplit() const {
}

size_t IndependenceSplitter::getChildCount() const {
}

Slice* IndependenceSplitter::makeChild(size_t number) {
}

void IndependenceSplitter::addToChildDecom(size_t number,
					   const Term& component) {
}

void IndependenceSplitter::generateDecom(DecomConsumer* consumer) {
}
