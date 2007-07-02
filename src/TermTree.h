#ifndef TERM_TREE_GUARD
#define TERM_TREE_GUARD

#include <list>
#include "Term.h"
#include "Allocator.h"

class Node;

class TermTree {
public:
  TermTree(const TermTree& tree);
  TermTree(const Term& threshold, unsigned int dimension, unsigned int position);
  TermTree(unsigned int dimension, unsigned int position = 0);
  ~TermTree();

  TermTree& operator=(const TermTree& tree);

 private:

 public:	    
  class TreeWalker {
  private:
    Node* node() const {
      return _stack[_level];
    }

  public:

    TreeWalker(TermTree& tree);
    ~TreeWalker();

    void goToRoot();
    void print(ostream& out) const;
    void insertUnder(Exponent exponent);
    void insertRightOf(Exponent exponent);

    void stepDown();
    bool canStepDown() const;
    void stepUp();
    bool canStepUp() const;
    void stepRight();
    bool canStepRight() const;
    bool preorderNext();
    void getTerm(Term& term) const;
    Exponent getExponent(unsigned int level) const;
    bool atLeaf() const;
    bool atRoot() const;
    Node* getNode() const;
    int getLevel() const;

  private:
    TermTree& _tree;
    unsigned int _level;
    unsigned int _maxLevel;
    Node** _stack;
  };

  // TODO: consider what happens if the tree is empty.
  class TreeIterator {
  public:
    TreeIterator(const TermTree& tree);
    TreeIterator& operator++();
    void getTerm(Term& term) const;
    Exponent getExponent(unsigned int position) const;
    bool atEnd() const;
    void print(ostream& out) const;

  private:
    TreeWalker _walker;
    const TermTree& _tree;
  };

  bool empty() const;

  void insert(const TermTree& tree);

  void insert(const Term& term);

  void removeDominators(const Term& term);

  void removeDivisors(const Term& term);

  void removeNonDivisors(const Term& term);

  bool getDivisor(const Term& term) const;

  bool getComparable(const Term& term) const;

  unsigned int getDimension() const;

  // does NOT support threshold!
  bool getDominator(const Term& term) const;

  void* getComparable(const Node* node, const Term& term, unsigned int position) const;

  // This function is really meant to return a bool, but for some
  // *bizarre* reason this works significantly faster if I use a void*
  // as a stand-in for a bool when using gcc 4.0.3. int works just as
  // badly as bool does. I have no idea why.
  void* getDivisor(const Node* node, const Term& term, unsigned int position) const;

  void* getDominator(const Node* node, const Term& term, unsigned int position) const;

  void clear();

  // Warning: This takes time linear in the number of generators.
  size_t size() const;

  void insertProjectionHelper(Term& term, const Node* node, unsigned int position);

  bool insertProjectionOf(const TermTree& tree, Exponent from, Exponent to);

  void lcmHelper(Term& leastCommonMultiple,
		 const Node* node, unsigned int position) const;

  void lcm(Term& leastCommonMultiple) const;

  void setThreshold(const Term& term);

  int getPosition() const;

  void print(ostream& out) const;

  bool equalsHelper(const Node* a, const Node* b, unsigned int position) const;

  bool operator==(const TermTree& tree) const;

  static void clearStaticAllocator();

private:
  void insert(Node* node, const Term& term, unsigned int position);
  void copy(const Node* from, Node* to, unsigned int position);

  // These return true iff the node should be removed.
  bool removeDominators(Node* node, const Term& term, unsigned int position);
  bool removeDivisors(Node* node, const Term& term, unsigned int position);
  void removeNonDivisors(Node* node, const Term& term, unsigned int position);

  void print(const Node* node, ostream& out, unsigned int position) const;


#ifdef DEBUG
 public:
  void validate() const;
  void validate(const Node* node, unsigned int position) const;
#endif


  unsigned int _dimension;
  int _position;
  Term _threshold;
  Term _term;
  void* _allocator; // TODO: make this nicer
  Node* _root;
};

#endif
