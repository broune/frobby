#ifndef BIG_IDEAL_GUARD
#define BIG_IDEAL_GUARD

#include "VarNames.h"
#include <vector>

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
  mpz_class& getExponent(size_t term, size_t var);
  void setExponent(size_t term, size_t var, const mpz_class& exp);

  bool operator==(const BigIdeal& b) const;

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

  void print(FILE* file) const;

private:
  static bool bigTermCompare(const vector<mpz_class>& a,
			     const vector<mpz_class>& b);

  vector<string> _variables;
  vector<vector<mpz_class> > _terms;
  VarNames _names;
};

#endif
