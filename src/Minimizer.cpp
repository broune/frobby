#include "stdinc.h"
#include "Minimizer.h"

#include "Term.h"

typedef vector<Exponent*>::iterator iterator;

::iterator simpleMinimize(::iterator begin, ::iterator end, size_t varCount) {
  if (distance(begin, end) <= 1)
	return end;

  std::sort(begin, end, Term::LexComparator(varCount));

  // TODO: use that least element does not need a check

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

::iterator twoVarMinimize(::iterator begin, ::iterator end) {
  if (begin == end)
	return end;

  std::sort(begin, end, Term::LexComparator(2));

  ::iterator last = begin;
  ::iterator it = begin;
  ++it;
  for (; it != end; ++it) {
	if ((*it)[1] < (*last)[1]) {
	  ++last;
	  *last = *it;
	}
  }

  ++last;
  return last;
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
  if (_varCount == 2)
	return twoVarMinimize(begin, end);
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

Minimizer::iterator Minimizer::colonReminimize
(iterator begin, iterator end, const Term& colon) {
  ASSERT(simpleIsMinimal(begin, end));

  if (colon.getSizeOfSupport() == 1) {
    size_t var = colon.getFirstNonZeroExponent();
    return colonReminimize(begin, end, var, colon[var]);
  }

  if (distance(begin, end) >= 100) {
	for (iterator it = begin; it != end; ++it) {
	  if (colon.strictlyDivides(*it)) {
		::colon(*it, *it, colon.begin(), _varCount);
		swap(*begin, *it);
		++begin;
		continue;
	  }
	}
  }
  
  if (colon.getSizeOfSupport() <= 2) {
	for (size_t var = 0; var < _varCount; ++var) {
	  if (colon[var] > 0)
		end = colonReminimize(begin, end, var, colon[var]);
	}
	return end;
  }

  for (iterator it = begin; it != end; ++it)
	::colon(*it, *it, colon.begin(), _varCount);

  return minimize(begin, end);
}

bool Minimizer::simpleIsMinimal(iterator begin, iterator end) {
  for (iterator a = begin; a != end; ++a)
	for (iterator b = begin; b != end; ++b)
	  if (::divides(*a, *b, _varCount) && a != b)
		return false;
  return true;
}

bool Minimizer::dominatesAny
(iterator begin, iterator end, const Exponent* term) {
  for (; begin != end; ++begin)
	if (::dominates(term, *begin, _varCount))
	  return true;
  return false;
}

bool Minimizer::dividesAny
(iterator begin, iterator end, const Exponent* term) {
  for (; begin != end; ++begin)
	if (::divides(term, *begin, _varCount))
	  return true;
  return false;
}


Minimizer::iterator Minimizer::colonReminimizePreprocess
(iterator begin, iterator end, const Term& colon) {
  for (iterator it = begin; it != end; ++it) {
	if (colon.strictlyDivides(*it)) {
	  ::colon(*it, *it, colon.begin(), _varCount);
	  swap(*begin, *it);
	  ++begin;
	}
  }
  return begin;
}

Minimizer::iterator Minimizer::colonReminimize
(iterator begin, iterator end, size_t var, Exponent exponent) {
  std::sort(begin, end,
			Term::DescendingSingleDegreeComparator(var, _varCount));

  // These can be kept without any further processing at all.
  while (begin != end && (*begin)[var] > exponent) {
    (*begin)[var] -= exponent; // perform colon
    ++begin;
  }

  if (begin == end)
	return end;

  // We group terms into blocks according to term[var].
  iterator previousBlockEnd = begin;
  iterator newEnd = begin;

  Exponent block = (*begin)[var];
  if (block == 0)
	return end;

  for (iterator it = begin; it != end; ++it) {
    // Detect if we are moving on to next block.
    if ((*it)[var] != block) {
      block = (*it)[var];
      previousBlockEnd = newEnd;
    }

	ASSERT((*it)[var] <= exponent);
	(*it)[var] = 0;

    bool remove = false;

	// TODO: find a better names for it and it2
    for (iterator it2 = begin; it2 != previousBlockEnd; ++it2) {
      if (::divides(*it2, *it, _varCount)) {
		remove = true;
		break;
      }
    }

    if (!remove) {
      *newEnd = *it;
      ++newEnd;
    }
  }

  return newEnd;
}
