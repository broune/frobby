#ifndef BIG_IDEAL_GUARD
#define BIG_IDEAL_GUARD

#include "VarNames.h"

class TermTree;
class TermList;
class TermTranslator;

class BigIdeal {
public:
  BigIdeal();
  BigIdeal(const VarNames& names);

  void insert(const TermTree& tree);
  void insert(const TermList& termList);

  void setNames(const VarNames& names);

  void newLastTerm();

  // TODO: move to some facade.
  void minimize();

  mpz_class& getLastTermExponentRef(size_t var);

  bool operator==(const BigIdeal& b) const;

  // Returns the number of artinian generators that have been added
  // that do not come from the input itself.
  size_t buildAndClear(TermTree*& tree,
		       TermTranslator*& translator,
		       bool generisize,
		       bool artinize = true);

  size_t size() const;

  vector<mpz_class>& operator[](unsigned int index);

  const vector<mpz_class>& operator[](unsigned int index) const;

  bool empty() const;

  void clear();

  // TODO: remove
  size_t getGeneratorCount() const;

  void clearAndSetNames(const VarNames& names);

  const VarNames& getNames() const;

  // Sorts and also removes duplicates. Returns true if there were
  // duplicates.
  // TODO: also sort variables and reorder exponents accordingly.
  bool sortUnique();

  void print(ostream& out) const;

private:
  static bool bigTermCompare(const vector<mpz_class>& a,
			     const vector<mpz_class>& b);

  void makeGeneric();

  vector<vector<mpz_class> >*
  buildDecompressionMaps(const vector<map<mpz_class, Exponent> >&
			 compressionMaps,
			 bool generisized);

  TermTree*
  buildIdeal(vector<map<mpz_class, Exponent> >& compressionMaps,
	     vector<vector<mpz_class> >& decompressionMaps,
	     bool artinize);

  void makeCompressionMap(int position,
			  map<mpz_class, Exponent>& compressionMap);

  vector<string> _variables;
  vector<vector<mpz_class> > _terms;
  VarNames _names;
};

#endif
