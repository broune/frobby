#include "stdinc.h"
#include "BigIdeal.h"

#include "VarNames.h"
#include "TermList.h"
#include "TermTranslator.h"
#include "Ideal.h"

#include <map>

BigIdeal::BigIdeal() {
}

BigIdeal::BigIdeal(const VarNames& names):
  _names(names) {
}

void BigIdeal::insert(const Ideal& ideal) {
  Ideal::const_iterator it = ideal.begin();
  for (; it != ideal.end(); ++it) {
    newLastTerm();

    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = (*it)[var];
  }
}

void BigIdeal::insert(const Ideal& ideal,
		      const TermTranslator& translator) {
  Ideal::const_iterator it = ideal.begin();
  for (; it != ideal.end(); ++it) {
    newLastTerm();

    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = translator.getExponent(var, (*it)[var]);
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

mpz_class& BigIdeal::getLastTermExponentRef(size_t var) {
  ASSERT(!empty());
  ASSERT(var < _names.getVarCount());
  
  return _terms.back()[var];
}

bool BigIdeal::operator==(const BigIdeal& b) const {
  return _terms == b._terms;
}

size_t BigIdeal::buildAndClear(Ideal*& ideal,
			       TermTranslator*& translator,
			       bool generisize,
			       bool artinize) {
  size_t initialSize = getGeneratorCount();
  if (generisize)
    makeGeneric();

  vector<map<mpz_class, Exponent> > compressionMaps(_names.getVarCount());
  for (size_t variable = 0; variable < _names.getVarCount(); ++variable)
    makeCompressionMap(variable, compressionMaps[variable]);

  vector<vector<mpz_class> >* decompressionMaps =
    buildDecompressionMaps(compressionMaps,
			   generisize ? _terms.size() : 0,
			   _names.getVarCount());

  ideal = buildIdeal(this,
		    compressionMaps, *decompressionMaps,
		    _names.getVarCount(), artinize);


  translator = new TermTranslator(_names, decompressionMaps);

  _terms.clear();
  return ideal->getGeneratorCount() - initialSize;
}

TermTranslator* BigIdeal::buildAndClear
(const vector<BigIdeal*>& bigIdeals,
 vector<Ideal*>& ideals) {
  ASSERT(!bigIdeals.empty());
  ideals.clear();

  size_t varCount = bigIdeals[0]->_names.getVarCount();

  vector<map<mpz_class, Exponent> > compressionMaps(varCount);
  for (size_t variable = 0; variable < varCount; ++variable)
    makeCompressionMap(variable, bigIdeals, compressionMaps[variable]);

  vector<vector<mpz_class> >* decompressionMaps =
    buildDecompressionMaps(compressionMaps, 0, varCount);

  for (size_t i = 0; i < bigIdeals.size(); ++i) {
    Ideal* ideal =
      buildIdeal(bigIdeals[i], compressionMaps, *decompressionMaps,
		 varCount, false);

    ideals.push_back(ideal);
  }

  return new TermTranslator(bigIdeals[0]->_names, decompressionMaps);
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

size_t BigIdeal::getVarCount() const {
  return _names.getVarCount();
}

void BigIdeal::clearAndSetNames(const VarNames& names) {
  clear();
  _names = names;
}

const VarNames& BigIdeal::getNames() const {
  return _names;
}

void BigIdeal::sortGeneratorsUnique() {
  sortGenerators();
  vector<vector<mpz_class> >::iterator newEnd =
    unique(_terms.begin(), _terms.end());
  _terms.erase(newEnd, _terms.end());
}

void BigIdeal::sortGenerators() {
  std::sort(_terms.begin(), _terms.end(), bigTermCompare);
}

struct VarSorter {
  VarSorter(VarNames& names):
    _names(names) {
    for (size_t i = 0; i < names.getVarCount(); ++i)
      _permutation.push_back(i);
    sort(_permutation.begin(), _permutation.end(), *this);
  }

  bool operator()(size_t a, size_t b) {
    return
      _names.getName(_permutation[a]) <
      _names.getName(_permutation[b]);
  }

  void getOrderedNames(VarNames& names) {
    names.clear();
    for (size_t i = 0; i < _permutation.size(); ++i)
      names.addVar(_names.getName(_permutation[i]));
  }

  void permute(vector<mpz_class>& term) {
    _tmp = term;
    for (size_t i = 0; i < _permutation.size(); ++i)
      term[i] = _tmp[_permutation[i]];
  }

private:
  vector<size_t> _permutation;
  VarNames _names;
  vector<mpz_class> _tmp;
};

void BigIdeal::sortVariables() {
  VarSorter sorter(_names);
  sorter.getOrderedNames(_names);
  for (size_t i = 0; i < _terms.size(); ++i)
    sorter.permute(_terms[i]);
}

void BigIdeal::print(ostream& out) const {
  out << "/---- BigIdeal of " << _terms.size() << " terms:" << endl;
  for (vector<vector<mpz_class> >::const_iterator it = _terms.begin();
       it != _terms.end(); ++it) {
    for (vector<mpz_class>::const_iterator entry = it->begin();
	 entry != it->end(); ++entry)
      out << *entry << ' ';
    out << '\n';
  }
  out << "----/ End of list." << endl;
}

const mpz_class& BigIdeal::getExponent(size_t term, size_t var) const {
  ASSERT(term < _terms.size());
  ASSERT(var < _names.getVarCount());

  return _terms[term][var];
}

void BigIdeal::setExponent(size_t term, size_t var, const mpz_class& exp) {
  ASSERT(term < _terms.size());
  ASSERT(var < _names.getVarCount());

  _terms[term][var] = exp;
}

bool BigIdeal::bigTermCompare(const vector<mpz_class>& a,
			      const vector<mpz_class>& b) {
  ASSERT(a.size() == b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] > b[i])
      return true;
    if (a[i] < b[i])
      return false;
  }
  return false;
}

void BigIdeal::makeGeneric() {
  std::sort(_terms.begin(), _terms.end());

  for (unsigned int i = 0; i < _terms.size(); ++i)
    for (unsigned int j = 0; j < _terms[i].size(); ++j)
      if (_terms[i][j] != 0)
	_terms[i][j] = _terms[i][j] * _terms.size() + i;
}

vector<vector<mpz_class> >*
BigIdeal::buildDecompressionMaps
(const vector<map<mpz_class, Exponent> >&
 compressionMaps,
 size_t generisized, // 0 if no generisizing, _terms.size() otherwise.
 size_t varCount) {
  vector<vector<mpz_class> >* decompressionMaps =
    new vector<vector<mpz_class> >(varCount);

  for (size_t variable = 0; variable < varCount; ++variable) {
    const map<mpz_class, Exponent>& compressionMap =
      compressionMaps[variable];
    vector<mpz_class>& decompressionMap = (*decompressionMaps)[variable];

    // The +1 is for the Artinian power we add later.
    decompressionMap.reserve(compressionMap.size() + 1);
    map<mpz_class, Exponent>::const_iterator it = compressionMap.begin();
    for (; it != compressionMap.end(); ++it) {
      ASSERT(it->second == decompressionMap.size());
      if (generisized != 0)
	decompressionMap.push_back(it->first / generisized);
      else
	decompressionMap.push_back(it->first);
    }

    // For the possible added artinian power.
    decompressionMap.push_back(0);
  }

  return decompressionMaps;
}

Ideal*
BigIdeal::buildIdeal(BigIdeal* bigIdeal,
                     vector<map<mpz_class, Exponent> >& compressionMaps,
		     vector<vector<mpz_class> >& decompressionMaps,
                     size_t varCount,
		     bool artinize) {
  vector<bool> hasArtinianPower(varCount);
  Ideal* ideal = new TermList(varCount);

  // Populate hasArtinianPower and ideal with data.
  Term term(varCount);
  for (size_t i = 0; i < bigIdeal->_terms.size(); ++i) {
    int artinianVariable = -1;
    for (size_t variable = 0; variable < varCount; ++variable) {
      term[variable] = compressionMaps[variable][bigIdeal->_terms[i][variable]];
      if (term[variable] != 0) {
	if (artinianVariable == -1)
	  artinianVariable = variable;
	else
	  artinianVariable = -2;
      }
    }
    ideal->insert(term);

    if (artinianVariable >= 0)
      hasArtinianPower[artinianVariable] = true;
  }

  if (artinize) {
    // Add any missing Artinian powers.
    for (size_t variable = 0; variable < varCount; ++variable) {
      if (hasArtinianPower[variable])
	continue;

      Exponent power = decompressionMaps[variable].size() - 1;
      term.setToIdentity();
      term[variable] = power;
      ideal->insert(term);
    }
  }

  return ideal;
}

void BigIdeal::makeCompressionMap
(int position,
 const vector<BigIdeal*> ideals,
 map<mpz_class, Exponent>& compressionMap) {
  // Collect the exponents.
  vector<mpz_class> exponents;
  for (size_t i = 0; i < ideals.size(); ++i)
    for (size_t j = 0; j < ideals[i]->_terms.size(); ++j)
      exponents.push_back(ideals[i]->_terms[j][position]);

  makeCompressionMap(exponents, compressionMap);
}

void BigIdeal::makeCompressionMap(int position,
				  map<mpz_class, Exponent>& compressionMap) {
  // Collect the exponents.
  vector<mpz_class> exponents;
  exponents.reserve(_terms.size());
  for (size_t i = 0; i < _terms.size(); ++i)
    exponents.push_back(_terms[i][position]);

  makeCompressionMap(exponents, compressionMap);
}

void BigIdeal::makeCompressionMap
    (vector<mpz_class>& exponents,
     map<mpz_class, Exponent>& compressionMap) {
  // Sort the exponents and remove duplicates.
  std::sort(exponents.begin(), exponents.end());
  vector<mpz_class>::iterator uniqueEnd =
    unique(exponents.begin(), exponents.end());
  exponents.erase(uniqueEnd, exponents.end());

  if (exponents.empty())
    return;

  // Construct the map from large exponents to small id numbers. The
  // map preserves order.
  compressionMap.clear();
  compressionMap[0] = 0;
  Exponent maxId = 0;

  size_t startingIndex = (exponents[0] == 0 ? 1 : 0);
  for (size_t i = startingIndex; i < exponents.size(); ++i)
    compressionMap[exponents[i]] = ++maxId;
}
