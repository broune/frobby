#include "../stdinc.h"
#include "TermTree.h"

#include "../Ideal.h"

class Node {
public:
  Node(): _entry(0), _next(0), _exponent(0) {}
  ~Node() {}
    
  typedef Node Entry;

  void clear() {
    _entry = 0;
  }

  class Iterator {
  public:
    Iterator(Entry* entry): _entry(entry) {}

    Iterator& operator++() {
      _entry = _entry->_next;
      return *this;
    }

    Iterator operator++(int) {
      Entry* oldValue = _entry;
      _entry = _entry->_next;
      return Iterator(oldValue);
    }

    bool operator==(const Iterator it) const {
      return _entry == it._entry;
    }

    bool operator!=(const Iterator it) const {
      return _entry != it._entry;
    }

    Entry* operator->() {
      return _entry;
    }

    Entry& operator*() {
      return *_entry;
    }

  private:
    Entry* _entry;
  };

  typedef Iterator iterator;

  iterator begin() const {return Iterator((Entry*)_entry);}
  iterator end() const {return Iterator(0);}

  bool empty() const {return _entry == 0;}

  iterator eraseAfter(Entry* entry) {
    if (entry == 0) {
      _entry = _entry->_next;
      return Iterator(_entry);
    }

    entry->_next = entry->_next->_next;
    return Iterator(entry->_next);
  }

  iterator getOrInsertChild(Exponent exponent, Allocator<Node>* allocator) {
    Entry** previousEntry = &_entry;
    iterator it = begin();
    for (; &*it != 0; ++it) {
      if (it->getExponent() >= exponent) {
	if (it->getExponent() == exponent)
	  return it;
	else
	  break;
      }
      previousEntry = &(it->_next);
    }

    Entry* entry = allocator->allocate();
    entry->_entry = 0;
    entry->_exponent = exponent;
    entry->_next = &*it;

    *previousEntry = entry;

    ASSERT(entry->_next != entry);
    return Iterator(entry);
  }

  Exponent getExponent() {
    return _exponent;
  }

  Node* getChild() const {
    return (Node*)_entry;
  }

  Node* getNext() const {
    return _next;
  }

  Entry* _entry;
  Entry* _next;
  Exponent _exponent;
};

TermTree::TermTree(const TermTree& tree):
  _dimension(tree._dimension),
  _position(tree._position),
  _threshold(tree._threshold),
  _term(tree._dimension),
  _allocator(new Allocator<Node>()),
  _root(((Allocator<Node>*)_allocator)->allocate()) {
  _root->clear();
  *this = tree;
}

TermTree& TermTree::operator=(const TermTree& tree) {
  ASSERT(_dimension == tree._dimension);
  clear();

  _position = tree._position;
  _threshold = tree._threshold;

  if (!tree.empty())
    copy(tree._root, _root, tree._position);
    
  return *this;
}

TermTree::TermTree(const Term& threshold, unsigned int dimension, unsigned int position):
  _dimension(dimension),
  _position(position),
  _threshold(threshold),
  _term(dimension),
  _allocator(new Allocator<Node>()),
  _root(((Allocator<Node>*)_allocator)->allocate()) {
  _root->clear();
}

TermTree::TermTree(unsigned int dimension, unsigned int position):
  _dimension(dimension),
  _position(position),
  _threshold(dimension),
  _term(dimension),
  _allocator(new Allocator<Node>()),
  _root(((Allocator<Node>*)_allocator)->allocate()) {
  _root->clear();
}
	    
TermTree::~TermTree() {
  delete (Allocator<Node>*)_allocator;
}

void TermTree::getTerms(Ideal& terms) const {
  TreeIterator it(*this);
  Term term(_dimension);
  while (!it.atEnd()) {
    it.getTerm(term);
    terms.insert(term);
    ++it;
  }
}

void TermTree::insert(const Ideal& ideal) {
  ASSERT(_dimension == ideal.getVarCount());
  Term tmp(_dimension);

  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator it = ideal.begin(); it != stop; ++it) {
    tmp = *it;
    insert(tmp);
  }
}


TermTree::TreeWalker::TreeWalker(TermTree& tree):
  _tree(tree),
  _level(0),
  _maxLevel(tree._dimension - tree._position),
  _stack(new Node*[tree._dimension + 1]) {
  _stack[0] = tree._root;
}

TermTree::TreeWalker::~TreeWalker() {
  delete[] _stack;
}

void TermTree::TreeWalker::goToRoot() {
  _level = 0;
}

void TermTree::TreeWalker::print(FILE* file) const {
  fprintf(file, "Walker(level=%lu, maxLevel=%lu, stack=[",
	  (unsigned long)_level, (unsigned long)_maxLevel);

  for (unsigned int i = 0; i <= _level; ++i) {
    if (i != 0)
      fputc(',', stderr);
    fprintf(stderr, "(%p E%lu)",
	    (void*)_stack[i], (unsigned long)_stack[i]->_exponent);
  }
  fputs("]\n", stderr);
}

void TermTree::TreeWalker::insertUnder(Exponent exponent) {
  ASSERT(!atLeaf());
  
  Node* child = ((Allocator<Node>*)_tree._allocator)->allocate();
  child->_entry = 0;
  child->_next = node()->_entry;
  child->_exponent = exponent;
  
  node()->_entry = child;
}

void TermTree::TreeWalker::insertRightOf(Exponent exponent) {
  ASSERT(node()->_exponent < exponent);
  ASSERT(node()->_next == 0 || exponent < node()->_next->_exponent);
      
  Node* child = ((Allocator<Node>*)_tree._allocator)->allocate();
  child->_entry = 0;
  child->_next = node()->_next;
  child->_exponent = exponent;

  node()->_next = child;
}

void TermTree::TreeWalker::stepDown() {
  ASSERT(canStepDown());
  ASSERT(node()->_entry != 0);
  
  _stack[_level + 1] = node()->_entry;
  ++_level;
}

bool TermTree::TreeWalker::canStepDown() const {
  return _level != _maxLevel;
}

void TermTree::TreeWalker::stepUp() {
  ASSERT(canStepUp());
  
  --_level;
}

bool TermTree::TreeWalker::canStepUp() const {
  return _level != 0;
}

void TermTree::TreeWalker::stepRight() {
  ASSERT(canStepRight());
  
  _stack[_level] = node()->_next;
}

bool TermTree::TreeWalker::canStepRight() const {
  return node()->_next != 0;
}

bool TermTree::TreeWalker::preorderNext() {
  if (canStepDown()) {
    stepDown();
    return true;
  }

  while (!canStepRight()) {
    if (!canStepUp())
      return false;
    stepUp();
  }

  stepRight();
  return true;
}

void TermTree::TreeWalker::getTerm(Term& term) const {
  int base = term.getVarCount() - _maxLevel - 1;
  for (unsigned int l = 1; l <= _level; ++l)
    term[base + l] = _stack[l]->_exponent;
}

Exponent TermTree::TreeWalker::getExponent(unsigned int level) const {
  ASSERT(level <= _level);
  return _stack[level]->getExponent();
}

bool TermTree::TreeWalker::atLeaf() const {
  return _level == _maxLevel;
}

bool TermTree::TreeWalker::atRoot() const {
  return _level == 0;
}

Node* TermTree::TreeWalker::getNode() const {
  return node();
}

int TermTree::TreeWalker::getLevel() const {
  return _level;
}

TermTree::TreeIterator::TreeIterator(const TermTree& tree):
  _walker(const_cast<TermTree&>(tree)),
  _tree(tree) {
  if (!tree.empty()) {
    while (true) {
      if (!_walker.canStepDown())
	break;
      _walker.stepDown();
    }
  }
}

TermTree::TreeIterator& TermTree::TreeIterator::operator++() {
  ASSERT(!atEnd());
  ASSERT(_walker.atLeaf());
      
  _walker.preorderNext();

  if (!atEnd()) {
    while (_walker.canStepDown())
      _walker.stepDown();
  }
  return *this;
}

void TermTree::TreeIterator::getTerm(Term& term) const {
  ASSERT(!atEnd());
  ASSERT(!_walker.canStepDown());
  _walker.getTerm(term);
}

Exponent TermTree::TreeIterator::getExponent(unsigned int position) const {
  unsigned int level = position + 1 - _tree.getPosition();
  return _walker.getExponent(level);
}

bool TermTree::TreeIterator::atEnd() const {
  return _walker.atRoot();
}

void TermTree::TreeIterator::print(FILE* file) const {
  _walker.print(file);
}

bool  TermTree::empty() const {
  return _root->empty();
}

void  TermTree::insert(const TermTree& tree) {
  ASSERT(_position == tree._position);

  TreeIterator it(tree);

  while (!it.atEnd()) {
    it.getTerm(_term);
    if (getDivisor(_term) == 0) {
      removeDominators(_term);
      insert(_root, _term, _position);
    }
    ++it;
  }
}

void TermTree::insert(const Term& term) {
  PROFILE_NO_INLINE;
  insert(_root, term, _position);
}

void TermTree::removeDominators(const Term& term) {
  removeDominators(_root, term, _position);
}

void TermTree::removeDivisors(const Term& term) {
  removeDivisors(_root, term, _position);
}

void TermTree::removeNonDivisors(const Term& term) {
  removeNonDivisors(_root, term, _position);
}

bool TermTree::getDivisor(const Term& term) const {
  return getDivisor(_root, term, _position);
}

bool TermTree::getComparable(const Term& term) const {
  return getComparable(_root, term, _position);
}

// does NOT support threshold!
bool TermTree::getDominator(const Term& term) const {
  return getDominator(_root, term, _position);
}

void* TermTree::getComparable(const Node* node, const Term& term, unsigned int position) const {
  PROFILE_NO_INLINE;
  ASSERT(position <= _dimension);
  
  if (position == _dimension)
    return (void*)1; // in this case we have equality.

  Exponent exponent = term[position];
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it->getExponent() == exponent) {
      if (getComparable(&*it, term, position + 1))
	return (void*)1;
    }	else if (it->getExponent() > exponent) {
      if (getDominator(&*it, term, position + 1))
	return (void*)1;
    }	else if (it->getExponent() < exponent) {
      if (getDivisor(&*it, term, position + 1))
	return (void*)1;
    }
  }
  return (void*)0;
}

unsigned int TermTree::getDimension() const {
  return _dimension;
}

// This function is really meant to return a bool, but for some
// *bizarre* reason this works significantly faster if I use a void*
// as a stand-in for a bool when using gcc 4.0.3. int works just as
// badly as bool does. I have no idea why.
void* TermTree::getDivisor(const Node* node, const Term& term, unsigned int position) const {
  PROFILE_NO_INLINE;
  ASSERT(position <= _dimension);

  if (position == _dimension)
    return (void*)1;

  Exponent exponent = term[position];
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it->getExponent() > exponent)
      break;
    if (getDivisor(&*it, term, position + 1))
      return (void*)1;
  }
  return (void*)0;
}

void* TermTree::getDominator(const Node* node, const Term& term, unsigned int position) const {
  PROFILE_NO_INLINE;
  ASSERT(position <= _dimension);

  if (position == _dimension)
    return (void*)1;

  Exponent exponent = term[position];
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it->getExponent() < exponent)
      continue;
    if (getDominator(&*it, term, position + 1))
      return (void*)1;
  }
  return (void*)0;
}

void TermTree::clear() {
  ((Allocator<Node>*)_allocator)->clear();
  _root = ((Allocator<Node>*)_allocator)->allocate();
  _root->clear();
}

// Warning: This takes time linear in the number of generators.
size_t TermTree::size() const {
  size_t count = 0;
  TreeIterator it(*this);
  while (!it.atEnd()) {
    ++count;
    ++it;
  }
  
  return count;
}
  
void TermTree::insertProjectionHelper(Term& term, const Node* node, unsigned int position) {
  if (position == _dimension) {
    if (getDivisor(term) == 0) {
      removeDominators(term);
      insert(term);
    }
  } else {
    for (Node::iterator it = node->begin(); &*it != 0; ++it) {
      Exponent exponent = it->getExponent();
      Exponent threshold = _threshold[position];
      if (exponent <= threshold)
	exponent = 0;
	
      term[position] = exponent;

      insertProjectionHelper(term, &*it, position + 1);
    }
  }
}

bool TermTree::insertProjectionOf(const TermTree& tree, Exponent from, Exponent to) {
  if (tree.empty())
    return false;

  bool mayHaveChanged = false;
  ASSERT(_position == tree._position + 1);
  Term term(_dimension);
  for (Node::iterator it = tree._root->begin(); &*it != 0; ++it) {
    if (it->getExponent() > to)
      break;
    if (it->getExponent() >= from) {
      mayHaveChanged = true;
      insertProjectionHelper(term, &*it, _position);
    }
  }
  return mayHaveChanged;
}

void TermTree::lcmHelper(Term& leastCommonMultiple,
			 const Node* node, unsigned int position) const {
  if (position == _dimension)
    return;
  
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it->getExponent() > leastCommonMultiple[position] )
      leastCommonMultiple[position] = it->getExponent();
    lcmHelper(leastCommonMultiple, &*it, position + 1);
  }
}

void TermTree::lcm(Term& leastCommonMultiple) const {
  leastCommonMultiple.setToIdentity();
  lcmHelper(leastCommonMultiple, _root, _position);
}

void TermTree::gcdHelper(Term& greatestCommonDivisor,
			 const Node* node, unsigned int position) const {
  if (position == _dimension)
    return;
  
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it->getExponent() < greatestCommonDivisor[position] )
      greatestCommonDivisor[position] = it->getExponent();
    gcdHelper(greatestCommonDivisor, &*it, position + 1);
  }
}

void TermTree::gcd(Term& greatestCommonDivisor) const {
  for (size_t var = 0; var < _dimension; ++var)
    greatestCommonDivisor[var] = numeric_limits<Exponent>::max();
  gcdHelper(greatestCommonDivisor, _root, _position);
}

void TermTree::setThreshold(const Term& term) {
  _threshold = term;
}

int TermTree::getPosition() const {
  return _position;
}

void TermTree::print(FILE* file) const {
#ifdef DEBUG
  validate();
#endif

  fprintf(file, "TREE (pos=%lu, term=", (unsigned long)_position);
  _threshold.print(file);
  fputs("): ", file);

  print(_root, file, _position);
  fputc('\n', file);
}

bool TermTree::equalsHelper(const Node* a, const Node* b, unsigned int position) const {
  if (position == _dimension)
    return true;

  Node::iterator itA = a->begin();
  Node::iterator itB = b->begin();

  while (true) {
    if (&*itA == 0)
      return &*itB == 0;
    if (&*itB == 0)
      return false;

    if (itA->getExponent() != itB->getExponent())
      return false;

    if (!equalsHelper(&*itA, &*itB, position + 1))
      return false;

    ++itA;
    ++itB;
  }
}

bool TermTree::operator==(const TermTree& tree) const {
  ASSERT(_dimension == tree._dimension);
  if (_position != tree._position ||
      !(_threshold == tree._threshold))
    return false;

  if (empty())
    return tree.empty();
  if (tree.empty())
    return false;

  return equalsHelper(_root, tree._root, _position);
}

void TermTree::clearStaticAllocator() {
  Allocator<Node::Entry>::clearStatic();
}


void TermTree::insert(Node* node, const Term& term, unsigned int position) {
  ASSERT(position <= _dimension);
  
  if (position == _dimension)
    return;
  
  Exponent exponent = term[position];
  Exponent threshold = _threshold[position];
  if (exponent <= threshold)
    exponent = 0;
  
  Node* child = &*(node->getOrInsertChild(exponent, ((Allocator<Node>*)_allocator)));
  insert(child, term, position + 1);
}

void TermTree::copy(const Node* from, Node* to, unsigned int position) {
  ASSERT(position <= _dimension);
  
  while (true) {
    to->_exponent = from->_exponent;
    if (position == _dimension)
      to->_entry = from->_entry;
    else {
      to->_entry = ((Allocator<Node>*)_allocator)->allocate();
      copy(from->_entry, to->_entry, position + 1);
    }
    
    if (from->_next == 0) {
      to->_next = 0;
      break;
    }
    
    Node* next = ((Allocator<Node>*)_allocator)->allocate();
    to->_next = next;
    to = next;
    from = from->_next;
  }
}

// Returns true iff the node should be removed.
bool TermTree::removeDominators(Node* node, const Term& term, unsigned int position) {
  ASSERT(position <= _dimension);
  
  if (position == _dimension)
    return true;
    
  Exponent exponent = term[position];
  Exponent threshold = _threshold[position];
  if (exponent <= threshold)
    exponent = 0;
    
  Node::iterator it = node->begin();
  Node::Entry* previousEntry = 0;
  while (&*it != 0) {
    if (it->getExponent() >= exponent) {
      if (removeDominators(&*it, term, position + 1)) {
	it = node->eraseAfter(previousEntry);
	continue;
      }
    }
    previousEntry = &*it;
    ++it;
  }

  return node->empty();
}

bool TermTree::removeDivisors(Node* node, const Term& term, unsigned int position) {
  ASSERT(position <= _dimension);

  if (position == _dimension)
    return true;
    
  // superflous?
  Exponent exponent = term[position];
  Exponent threshold = _threshold[position];
  if (exponent <= threshold)
    exponent = 0;
    
  Node::iterator it = node->begin();
  Node::Entry* previousEntry = 0;
  while (&*it != 0) {
    if (it->getExponent() <= exponent) {
      if (removeDivisors(&*it, term, position + 1)) {
	it = node->eraseAfter(previousEntry);
	continue;
      }
    }
    previousEntry = &*it;
    ++it;
  }

  return node->empty();
}

// Returns true iff the node should be removed.
void TermTree::removeNonDivisors(Node* node, const Term& term, unsigned int position) {
  ASSERT(position <= _dimension);

  if (position == _dimension)
    return;
    
  Exponent exponent = term[position];
    
  Node::iterator it = node->begin();
  Node::Entry* previousEntry = 0;
  while (&*it != 0) {
    if (it->getExponent() > exponent) {
      it = node->eraseAfter(previousEntry);
      continue;
    } else
      removeNonDivisors(&*it, term, position + 1);

    previousEntry = &*it;
    ++it;
  }
}

void TermTree::print(const Node* node, FILE* file, unsigned int position) const {
  if (position == _dimension)
    return;

  fputc('[', file);
  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    if (it != node->begin())
      fputc(',', file);
    fprintf(file, "%lu", (unsigned long)it->getExponent());
    print(&*it, file, position + 1);
  }
  fputc(']', file);
}

#ifdef DEBUG
void TermTree::validate() const {
  if (_root->empty())
    return;
  validate(_root, _position);
}

void TermTree::validate(const Node* node, unsigned int position) const {
  if (position == _dimension)
    return;
  
  if (node->empty()) {
    fprintf(stderr, "Validation Error: Inner node "
	    "in TermTree has no children.\n"
	    "This is at pos=%lu\n"
	    "Complete tree:", (unsigned long)position);
    print(_root, stderr, _position);
    fputc('\n', stderr);
    ASSERT(false);

  }

  for (Node::iterator it = node->begin(); &*it != 0; ++it) {
    ASSERT(node != &*it);
    ASSERT(&*it != (&*it)->_next);
    validate(&*it, position + 1);
  }

}
#endif
