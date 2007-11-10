#ifndef BIG_IDEAL_GUARD
#define BIG_IDEAL_GUARD

#include "VarNames.h"

class TermTranslator;
class Ideal;

class BigIdeal {
public:


  BigIdeal();
  BigIdeal(const VarNames& names);

  void insert(const Ideal& ideal);
  void insert(const Ideal& ideal, const TermTranslator& translator);

  void setNames(const VarNames& names);

  void newLastTerm();

  mpz_class& getLastTermExponentRef(size_t var);

  const mpz_class& getExponent(size_t term, size_t var) const;
  void setExponent(size_t term, size_t var, const mpz_class& exp);

  bool operator==(const BigIdeal& b) const;

  // Returns the number of artinian generators that have been added
  // that do not come from the input itself.
  size_t buildAndClear(Ideal*& tree,
		       TermTranslator*& translator,
		       bool generisize,
		       bool artinize = true);

  static TermTranslator* buildAndClear(const vector<BigIdeal*>& bigIdeals,
				       vector<Ideal*>& ideals);

  vector<mpz_class>& operator[](unsigned int index);

  const vector<mpz_class>& operator[](unsigned int index) const;

  bool empty() const;

  void clear();

  size_t getGeneratorCount() const;
  size_t getVarCount() const;

  void clearAndSetNames(const VarNames& names);

  const VarNames& getNames() const;

  // Sorts the generators and removes duplicates.
  void sortGeneratorsUnique();

  // Sorts the generators.
  void sortGenerators();

  // Sorts the variables.
  void sortVariables();

  void print(ostream& out) const;

private:
  static bool bigTermCompare(const vector<mpz_class>& a,
			     const vector<mpz_class>& b);

  void makeGeneric();

  static vector<vector<mpz_class> >*
    buildDecompressionMaps
    (const vector<map<mpz_class, Exponent> >&
     compressionMaps,
     size_t generisized,
     size_t varCount);

    static Ideal*
    buildIdeal(BigIdeal* ideal,
	       vector<map<mpz_class, Exponent> >& compressionMaps,
	       vector<vector<mpz_class> >& decompressionMaps,
	       size_t varCount,
	       bool artinize);

    static void makeCompressionMap
      (int position,
       const vector<BigIdeal*> ideals,
       map<mpz_class, Exponent>& compressionMap);

    void makeCompressionMap(int position,
			    map<mpz_class, Exponent>& compressionMap);

    static void makeCompressionMap
      (vector<mpz_class>& exponents,
       map<mpz_class, Exponent>& compressionMap);

    vector<string> _variables;
    vector<vector<mpz_class> > _terms;
    VarNames _names;
};

#endif
