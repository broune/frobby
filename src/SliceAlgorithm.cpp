#include "stdinc.h"
#include "SliceAlgorithm.h"

#include "TermList.h"
#include "Term.h"
#include "VarNames.h"
#include "io.h"
#include "TermTranslator.h"
#include "Ideal.h"

// TODO: move
unsigned int level = 0;
vector<unsigned int> calls;

SliceAlgorithm::SliceAlgorithm(const Ideal& ideal,
			       const VarNames& names,
			       const TermTranslator* translator,
			       ostream& out):
  _names(names),
  _translator(translator),
  _out(out) {

  // TODO: improve output configurability.
  _ioHandler = IOHandler::createIOHandler("monos");
  _ioHandler->startWritingIdeal(_out, _names);

  if (!ideal.isZeroIdeal()) {
    TermList subtract(ideal.getVariableCount());
    Term multiply(ideal.getVariableCount());
    
    content(ideal, subtract, multiply);
  }

  _ioHandler->doneWritingIdeal(_out);
  _out << flush;

  // TODO: move
  unsigned int sum = 0;
  for (unsigned int level = 0; level < calls.size(); ++level) {
    sum += calls[level];
    cerr << "Level " << level + 1 << " had "
	 << calls[level] << " calls. " << endl;
  }
  cerr << endl << "Sum: " << sum << endl;
}

void SliceAlgorithm::labelSplit(const Ideal& ideal,
				const Ideal& subtract,
				const Term& multiply) {
  Term lcm(ideal.getVariableCount());
  ideal.getLcm(lcm);

  size_t var = 0;
  for (size_t i = 1; i < ideal.getVariableCount(); ++i)
    if (lcm[i] > lcm[var])
      var = i;

  // TODO: improve
  ((Ideal&)ideal).singleDegreeSort(var);
  
  Ideal* cumulativeSubtract = (Ideal*)&subtract;
  Term labelMultiply(ideal.getVariableCount());
  Term pivot(ideal.getVariableCount());

  for (Ideal::const_iterator it = ideal.begin(); it != ideal.end(); ++it) {
    if ((*it)[var] == 0)
      continue;

    pivot = *it;
    pivot[var] -= 1;

    Ideal* labelIdeal = ideal.createMinimizedColon(pivot);
    Ideal* labelSubtract = cumulativeSubtract->createMinimizedColon(pivot);
    labelMultiply.product(multiply, pivot);

    simplify(*labelIdeal, *labelSubtract, labelMultiply); 
    content(*labelIdeal, *labelSubtract, labelMultiply);
    Ideal::const_iterator next = it;
    ++next;
    if (next != ideal.end() && (*it)[var] == (*next)[var]) {
      if (cumulativeSubtract == &subtract)
	cumulativeSubtract = subtract.clone();
      cumulativeSubtract->insert(pivot);
    } else if (cumulativeSubtract != &subtract) {
      delete cumulativeSubtract;
      cumulativeSubtract = (Ideal*)&subtract;
    }

    delete labelSubtract;
    delete labelIdeal;
  }
}

void SliceAlgorithm::content(const Ideal& ideal,
			     const Ideal& subtract,
			     const Term& multiply) {
  if (ideal.getGeneratorCount() == 0 ||
      ideal.isZeroIdeal()) {
    return;
  }

  if (calls.size() == level)
    calls.push_back(0);
  ++calls[level];
  ++level;

  //cerr << "*********** at level " << level << ' ' << multiply << endl;
  //ideal.print();

  if (baseCase(ideal, subtract, multiply)) {
    --level;
    //    cerr << "was basecase" << endl;
    return;
  }

  if (false) {
    labelSplit(ideal, subtract, multiply);
    --level;
    return;
  }

  Term pivot(ideal.getVariableCount());
  getPivot(ideal, subtract, pivot);

  //  cerr << "pivot: " << pivot << endl;

  // Handle inner slice
  //  cerr << "inner slice" << endl;
  Ideal* innerIdeal = ideal.createMinimizedColon(pivot);
  Ideal* innerSubtract = subtract.createMinimizedColon(pivot);
  Term innerMultiply(ideal.getVariableCount());
  innerMultiply.product(multiply, pivot);

  simplify(*innerIdeal, *innerSubtract, innerMultiply);
  content(*innerIdeal, *innerSubtract, innerMultiply);

  delete innerIdeal;
  delete innerSubtract;

  // Handle outer slice
  //  cerr << "outer slice" << endl;
  Ideal* outerIdeal = ideal.clone();
  Ideal* outerSubtract = subtract.clone();
  Term outerMultiply(multiply);
  outerSubtract->insert(pivot);
  outerIdeal->removeStrictMultiples(pivot);

  simplify(*outerIdeal, *outerSubtract, outerMultiply);
  content(*outerIdeal, *outerSubtract, outerMultiply);

  delete outerIdeal;
  delete outerSubtract;
  
  //  cerr << "returnin" << endl;
  --level;
}

// TODO: make the pivot selection strategy a parameter.
void SliceAlgorithm::getPivot(const Ideal& ideal,
			      const Ideal& subtract,
			      Term& pivot) {
  ASSERT(!ideal.isZeroIdeal());
  /*
    pivot.setToIdentity();
    for (Ideal::const_iterator it = ideal.begin();
    it != ideal.end(); ++it) {
    bool squareFree = true;
    for (size_t i = 0; i < ideal.getVariableCount(); ++i)
    if ((*it)[i] > 1)
    squareFree = false;

    if (!squareFree) {
    pivot = *it;
    break;
    }
    }

    for (size_t i = 0; i < ideal.getVariableCount(); ++i)
    if (pivot[i] > 0)
    pivot[i] -= 1;
  */
  
  Term lcm(ideal.getVariableCount());
  ideal.getLcm(lcm);
  
  size_t maxOffset = 0;
  
  for (size_t i = 1; i < ideal.getVariableCount(); ++i)
    if (lcm[i] > lcm[maxOffset])
      maxOffset = i;

  pivot.setToIdentity();
  if (lcm[maxOffset] > 3)
    pivot[maxOffset] = lcm[maxOffset] / 3;
  else
    pivot[maxOffset] = lcm[maxOffset] / 2;
  
  ASSERT(pivot[maxOffset] > 0); 
  ASSERT(!ideal.contains(pivot));
  ASSERT(!subtract.contains(pivot));
}

bool SliceAlgorithm::baseCase(const Ideal& ideal,
			      const Ideal& subtract,
			      const Term& multiply) {
  if (ideal.isZeroIdeal())
    return true;

  Term lcm(ideal.getVariableCount());
  ideal.getLcm(lcm);

  // Check that each variable appears in some minimal generator.
  for (size_t i = 0; i < ideal.getVariableCount(); ++i)
    if (lcm[i] == 0)
      return true; // i can have no label, so no content

  // Ensure that ideal is square free.
  for (size_t i = 0; i < ideal.getVariableCount(); ++i)
    if (lcm[i] > 1)
      return false; // not square free

  // We have reached the square free base. The content is non-empty if
  // and only if ideal has the form <x_1, ..., x_n>.
  if (ideal.getGeneratorCount() != ideal.getVariableCount())
    return true;

  // This might appear to rest on an assumption that ideal is
  // minimized and thus has no duplicates. This is not the case, since
  // we checked that each variable appears in some minimal generator.
  for (Ideal::const_iterator it = ideal.begin(); it != ideal.end(); ++it) {
    int degree = 0;
    for (size_t i = 0; i < ideal.getVariableCount(); ++i)
      degree += (*it)[i];
    if (degree != 1)
      return true;
  }

  // TODO: move this elsewhere.
  vector<const char*> tmp(ideal.getVariableCount());
  for (unsigned int var = 0; var < ideal.getVariableCount(); ++var)
    tmp[var] = (_translator->getExponentString(var, multiply[var] + 1));
  _ioHandler->writeGeneratorOfIdeal(_out, tmp, _names);

  return true;
}

// TODO: move onto class
bool normalize(Ideal& ideal, const Ideal& subtract) {
  bool removedAny = false;
  for (Ideal::const_iterator it = subtract.begin(); it != subtract.end(); ++it)
    if (ideal.removeStrictMultiples(*it))
      removedAny = true;
  return removedAny;
}

class DoubleLcmFilter : public Ideal::FilterFunction {
public:
  DoubleLcmFilter(const Term& lcm):
    _lcm(lcm) {}

  virtual bool operator()(const Exponent* term) {
    int lcmExps = 0;
    for (size_t i = 0; i < _lcm.getVarCount(); ++i)
      if (term[i] == _lcm[i])
	++lcmExps;

    return lcmExps < 2;
  }

private:
  Term _lcm;
};

bool filterDoubleLcm(Ideal& ideal) {
  Term lcm(ideal.getVariableCount());
  ideal.getLcm(lcm);

  DoubleLcmFilter filter(lcm);
  return ideal.filter(filter);
}

void SliceAlgorithm::simplify(Ideal& ideal, Ideal& subtract, Term& multiply) const {
  normalize(ideal, subtract);

 beginning:
  bool changed = false;

  Term gcd(ideal.getVariableCount());
  ideal.getGcd(gcd);

  if (!gcd.isIdentity()) {
    ideal.colon(gcd);
    subtract.colon(gcd);
    multiply.product(multiply, gcd);
    changed = true;
  }

  while (true) {
    Ideal::const_iterator idealEnd = ideal.end();

    Term lcm(ideal.getVariableCount());
    for (size_t offset = 0; offset < ideal.getVariableCount(); ++offset) {
      Term tmp(ideal.getVariableCount());
      bool first = true;
      for (Ideal::const_iterator it = ideal.begin(); it != idealEnd; ++it) {
	if ((*it)[offset] == 0)
	  continue;
	if (first) {
	  tmp = *it;
	  first = false;
	} else
	  tmp.gcd(tmp, *it);
      }
      if (!first) {
	tmp[offset] -= 1;
	lcm.lcm(lcm, tmp);
      }
    }    
    
    if (!lcm.isIdentity()) {
      subtract.colon(lcm);
      changed = true;

      ideal.colon(lcm);
      ideal.minimize();
      multiply.product(multiply, lcm);
    } else
      break;
  }

  if (filterDoubleLcm(ideal))
    changed = true;

  if (changed) {
    subtract.minimize();
    if (normalize(ideal, subtract))
      goto beginning;
  }
}
