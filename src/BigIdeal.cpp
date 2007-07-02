#include "stdinc.h"
#include "BigIdeal.h"

#include "VarNames.h"
#include "TermTree.h"
#include "TermList.h"
#include "TermTranslator.h"

#include <map>

BigIdeal::BigIdeal() {
}

BigIdeal::BigIdeal(const VarNames& names):
  _names(names) {
}

void BigIdeal::insert(const TermTree& tree) {
  TermTree::TreeIterator it(tree);
  ExternalTerm term(_names.getVarCount());
  while (!it.atEnd()) {
    newLastTerm();
    it.getTerm(term);
    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = term[var];
    ++it;
  }
}

void BigIdeal::insert(const TermList& termList) {
  TermList::const_iterator it = termList.begin();
  for (; it != termList.end(); ++it) {
    newLastTerm();

    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = (*it)[var];
  }
}

void BigIdeal::setNames(const VarNames& names) {
  _names = names;
}

void BigIdeal::newLastTerm() {
  ASSERT(!_names.empty());
  
  _terms.resize(_terms.size() + 1);
  _terms.back().resize(_names.getVarCount());
}

void BigIdeal::minimize() {
  for (size_t i = 0; i < _terms.size();) {
    bool remove = false;
    for (size_t j = 0; j < _terms.size(); ++j) {
      if (i == j)
	continue;
      
      bool divides = true;
      for (size_t k = 0; k < _names.getVarCount(); ++k) {
	if (_terms[j][k] > _terms[i][k]) {
	  divides = false;
	  break;
	}
      }
      if (divides) {
	remove = true;
	break;
      }
    }
    if (remove) {
      _terms.erase(_terms.begin() + i);
      
    } else
      ++i;
  }
}


mpz_class& BigIdeal::getLastTermExponentRef(size_t var) {
  ASSERT(!empty());
  ASSERT(var < _names.getVarCount());
  
  return _terms.back()[var];
}

bool BigIdeal::operator==(const BigIdeal& b) const {
  return _terms == b._terms;
}

size_t BigIdeal::buildAndClear(TermTree*& tree,
			       TermTranslator*& translator,
			       bool generisize,
			       bool artinize) {
  size_t initialSize = size();
  if (generisize)
    makeGeneric();
  
  vector<map<mpz_class, Exponent> > compressionMaps(_names.getVarCount());
  for (size_t variable = 0; variable < _names.getVarCount(); ++variable)
    makeCompressionMap(variable, compressionMaps[variable]);

  vector<vector<mpz_class> >* decompressionMaps =
    buildDecompressionMaps(compressionMaps, generisize);
  tree = buildIdeal(compressionMaps, *decompressionMaps, artinize);
  translator = new TermTranslator(decompressionMaps);

  _terms.clear();
  return tree->size() - initialSize;
}

size_t BigIdeal::size() const {
  return _terms.size();
}

vector<mpz_class>& BigIdeal::operator[](unsigned int index) {
  ASSERT(index < _terms.size());
  return _terms[index];
}

const vector<mpz_class>& BigIdeal::operator[](unsigned int index) const {
  ASSERT(index < _terms.size());
  return _terms[index];
}

bool BigIdeal::empty() const {
  return _terms.empty();
}

void BigIdeal::clear() {
  _terms.clear();
}

size_t BigIdeal::getGeneratorCount() const {
  return _terms.size();
}

void BigIdeal::clearAndSetNames(const VarNames& names) {
  clear();
  _names = names;
}

const VarNames& BigIdeal::getNames() const {
  return _names;
}

bool BigIdeal::sortUnique() {
  sort(_terms.begin(), _terms.end(), bigTermCompare);
  vector<vector<mpz_class> >::iterator newEnd =
    unique(_terms.begin(), _terms.end());
  if (newEnd != _terms.end()) {
    _terms.erase(newEnd, _terms.end());
    return true;
  }
  else
    return false;
}

void BigIdeal::print(ostream& out) const {
  out << "BigTerm list of " << _terms.size() << " elements:" << endl;
  for (vector<vector<mpz_class> >::const_iterator it = _terms.begin(); it != _terms.end(); ++it) {
    for (vector<mpz_class>::const_iterator entry = it->begin(); entry != it->end(); ++entry)
      out << *entry << ' ';
    out << '\n';
  }
  out << "---- End of list." << endl;
}

bool BigIdeal::bigTermCompare(const vector<mpz_class>& a,
			      const vector<mpz_class>& b) {
  ASSERT(a.size() == b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] < b[i])
      return true;
    if (a[i] > b[i])
      return false;
  }
  return false;
}

void BigIdeal::makeGeneric() {
  sort(_terms.begin(), _terms.end());

  for (unsigned int i = 0; i < _terms.size(); ++i)
    for (unsigned int j = 0; j < _terms[i].size(); ++j)
      if (_terms[i][j] != 0)
	_terms[i][j] = _terms[i][j] * _terms.size() + i;
}

vector<vector<mpz_class> >*
BigIdeal::buildDecompressionMaps(const vector<map<mpz_class, Exponent> >&
				 compressionMaps,
				 bool generisized) {
  vector<vector<mpz_class> >* decompressionMaps =
    new vector<vector<mpz_class> >(_names.getVarCount());

  for (size_t variable = 0; variable < _names.getVarCount(); ++variable) {
    const map<mpz_class, Exponent>& compressionMap =
      compressionMaps[variable];
    vector<mpz_class>& decompressionMap = (*decompressionMaps)[variable];

    // The +1 is for the Artinian power we may add later.
    decompressionMap.reserve(compressionMap.size() + 1);
    map<mpz_class, Exponent>::const_iterator it = compressionMap.begin();
    for (; it != compressionMap.end(); ++it) {
      ASSERT(it->second == decompressionMap.size());
      if (generisized)
	decompressionMap.push_back(it->first / _terms.size());
      else
	decompressionMap.push_back(it->first);
    }
  }

  return decompressionMaps;
}

TermTree*
BigIdeal::buildIdeal(vector<map<mpz_class, Exponent> >& compressionMaps,
		     vector<vector<mpz_class> >& decompressionMaps,
		     bool artinize) {
  vector<bool> hasArtinianPower(_names.getVarCount());
  TermTree* tree = new TermTree(_names.getVarCount());

  // Populate hasArtinianPower and tree with data.
  ExternalTerm term(_names.getVarCount());
  for (size_t i = 0; i < _terms.size(); ++i) {
    int artinianVariable = -1;
    for (size_t variable = 0; variable < _names.getVarCount(); ++variable) {
      term[variable] = compressionMaps[variable][_terms[i][variable]];
      if (term[variable] != 0) {
	if (artinianVariable == -1)
	  artinianVariable = variable;
	else
	  artinianVariable = -2;
      }
    }
    tree->insert(term);

    if (artinianVariable >= 0)
      hasArtinianPower[artinianVariable] = true;
  }

  if (artinize) {
    // Add any missing Artinian powers.
    for (size_t variable = 0; variable < _names.getVarCount(); ++variable) {
      if (hasArtinianPower[variable])
	continue;
	
      Exponent power = decompressionMaps[variable].size();
      decompressionMaps[variable].push_back(0);
	
      term.setToZero();
      term[variable] = power;
      tree->insert(term);
    }
  }

  return tree;
}

void BigIdeal::makeCompressionMap(int position,
				  map<mpz_class, Exponent>& compressionMap) {
  // Collect the exponents.
  vector<mpz_class> exponents;
  exponents.reserve(_terms.size());
  for (size_t i = 0; i < _terms.size(); ++i)
    exponents.push_back(_terms[i][position]);

  // Sort the exponents and remove duplicates.
  sort(exponents.begin(), exponents.end());
  vector<mpz_class>::iterator uniqueEnd =
    unique(exponents.begin(), exponents.end());
  exponents.erase(uniqueEnd, exponents.end());

  // Construct the map from large exponents to small id numbers. The
  // map preserves order.
  compressionMap.clear();
  compressionMap[0] = 0;
  Exponent maxId = 0;

  size_t startingIndex = (exponents[0] == 0 ? 1 : 0);
  for (size_t i = startingIndex; i < exponents.size(); ++i)
    compressionMap[exponents[i]] = ++maxId;
}
