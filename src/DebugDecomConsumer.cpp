#include "stdinc.h"
#include "DebugDecomConsumer.h"

#include "Term.h"

DebugDecomConsumer::DebugDecomConsumer(DecomConsumer* consumer):
  _consumer(consumer) {
}

DebugDecomConsumer::~DebugDecomConsumer() {
  delete _consumer;
}  

void DebugDecomConsumer::consume(const Term& term) {
  cerr << "DEBUG: Writing " << term << " to output." << endl;
  _consumer->consume(term);
}
