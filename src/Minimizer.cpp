#include "stdinc.h"
#include "Minimizer.h"

#include "Term.h"

typedef vector<Exponent*>::iterator iterator;

::iterator simpleMinimize(::iterator begin, ::iterator end, size_t varCount) {
  std::sort(begin, end, Term::LexComparator(varCount));

  ::iterator newEnd = begin;
  for (::iterator dominator = begin; dominator != end; ++dominator) {

	bool remove = false;
	for (::iterator divisor = begin; divisor != newEnd; ++divisor) {
	  if (::divides(*divisor, *dominator, varCount)) {
		remove = true;
		break;
	  }
	}

	if (!remove) {
	  *newEnd = *dominator;
	  ++newEnd;
	}
  }
  return newEnd;
}

class TreeNode {
  typedef vector<Exponent*>::iterator iterator;

public:
  TreeNode(iterator begin, iterator end, size_t varCount):
	_lessOrEqual(0),
	_greater(0),
	_var(0),
	_pivot(0),
	_varCount(varCount),
	_begin(begin), 
	_end(end) {
  }

  ~TreeNode() {
	delete _lessOrEqual;
	delete _greater;
  }

  void makeTree() {
	ASSERT(_greater == 0);
	ASSERT(_lessOrEqual == 0);

	if (distance(_begin, _end) > 20) {

	  Term lcm(_varCount);
	  for (iterator it = _begin; it != _end; ++it)
		lcm.lcm(lcm, *it);

	  while (true) {
		size_t maxVar = 0;
		
		for (size_t var = 0; var < _varCount; ++var)
		  if (lcm[var] > lcm[maxVar])
			maxVar = var;
		if (lcm[maxVar] == 0) {
		  break; // we are not making any progress anyway
		}

		ASSERT(lcm[maxVar] >= 1);
		_var = maxVar;
		_pivot = lcm[maxVar] / 4;
		lcm[maxVar] = 0; // So we do not process this same var again.
	  
		iterator left = _begin;
		iterator right = _end - 1;
		while (left != right) {
		  while ((*left)[_var] <= _pivot && left != right)
			++left;
		  while ((*right)[_var] > _pivot && left != right)
			--right;
		  swap(*left, *right);
		}
		ASSERT((*right)[_var] > _pivot);
		if ((*_begin)[_var] > _pivot)
		  continue;

		iterator middle = right;
		while ((*middle)[maxVar] > _pivot)
		  --middle;
		++middle;
		
		ASSERT(middle != _begin);

		_lessOrEqual = new TreeNode(_begin, middle, _varCount);
		_greater = new TreeNode(middle, _end, _varCount);
		_end = _begin;

		_lessOrEqual->makeTree();
		_greater->makeTree();
		return;
	  }
	}

	_end = simpleMinimize(_begin, _end, _varCount);

  }

  bool isRedundant(const Exponent* term) {
	if (_begin != _end) {
	  ASSERT(_lessOrEqual == 0);
	  ASSERT(_greater == 0);

	  for (iterator it = _begin; it != _end; ++it)
		if (dominates(term, *it, _varCount))
		  return true;
	  return false;
	} else {
	  ASSERT(_lessOrEqual != 0);
	  ASSERT(_greater != 0);
	  
	  if (term[_var] > _pivot && _greater->isRedundant(term))
		return true;
	  if (_lessOrEqual->isRedundant(term))
		return true;
	  return false;
	}
  }

  void collect(vector<Exponent*>& terms) {
	if (_begin != _end) {
	  ASSERT(_lessOrEqual == 0);
	  ASSERT(_greater == 0);
	  terms.insert(terms.end(), _begin, _end);
	  return;
	}
	ASSERT(_lessOrEqual != 0);
	ASSERT(_greater != 0);

	size_t oldSize = terms.size();
	_greater->collect(terms);
	for (size_t i = oldSize; i < terms.size();) {
	  if (_lessOrEqual->isRedundant(terms[i])) {
		swap(terms[i], terms.back());
		terms.pop_back();
	  } else
		++i;
	}

	_lessOrEqual->collect(terms);
  }

  void print(FILE* out) {
	if (_begin == _end) {
	  ASSERT(_lessOrEqual != 0);
	  ASSERT(_greater != 0);

	  fprintf(out, "NODE (pivot=%lu^%lu, varCount = %lu\n",
			  (unsigned long)_var,
			  (unsigned long)_pivot,
			  (unsigned long)_varCount);
	  fputs("-lessOrEqual: ", out);
	  _lessOrEqual->print(out);
	  fputs("-greater: ", out);
	  _greater->print(out);
	  fputs(")\n", out);
	} else {
	  ASSERT(_lessOrEqual == 0);
	  ASSERT(_greater == 0);

	  fprintf(out, "NODE (_varCount = %lu terms:\n", (unsigned long)_varCount);
	  for (iterator it = _begin; it != _end; ++it) {
		fputc(' ', out);
		::print(out, *it, _varCount);
		fprintf(out, " %p\n", (void*)*it);
	  }
	  fputs(")\n", out);
	}
  }

private:
  TreeNode* _lessOrEqual;
  TreeNode* _greater;
  size_t _var;
  Exponent _pivot;
  size_t _varCount;

  iterator _begin;
  iterator _end;
};

Minimizer::iterator Minimizer::minimize(iterator begin, iterator end) const {
  if (distance(begin, end) < 1000 || _varCount == 0)
	return simpleMinimize(begin, end, _varCount);

  static vector<Exponent*> terms;
  terms.clear();
  terms.reserve(distance(begin, end));

  TreeNode node(begin, end, _varCount);
  node.makeTree();
  node.collect(terms);

  return copy(terms.begin(), terms.end(), begin);
}
