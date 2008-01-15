#ifndef TERM_TRANSLATOR_GUARD
#define TERM_TRANSLATOR_GUARD

#include "VarNames.h"

#include <vector>

class BigIdeal;
class Ideal;
class Term;

// TermTranslator handles translation between terms whose exponents
// are infinite precision integers and terms whose exponents are 32
// bit integers.
//
// This is done by assigning the big integers IDs that are 32 bit
// integers such that the assignment of IDs preserves order of
// exponents for each variable.
//
// The translation is done at the level of whole ideals.
//
// The big integer 0 is always assigned the ID 0.
class TermTranslator {
public:
  // The constructors translate BigIdeals into Ideals, while
  // initializing this to do the reverse translation. sortVars
  // indicates whether or not the order of the variable names should
  // be sorted. This cannot be turned off for the version taking
  // several ideals.
  TermTranslator(const BigIdeal& bigIdeal, Ideal& ideal, bool sortVars = true);
  TermTranslator(const vector<BigIdeal*>& bigIdeals, vector<Ideal*>& ideals);
  ~TermTranslator();

  // These methods translate from IDs to infinite precision integers.
  const mpz_class& getExponent(int variable, Exponent exponent) const;
  const mpz_class& getExponent(int variable, const Term& term) const;

  // As getExponent, except the string "[var]^[e]" is returned.
  // makeStrings must be called before getExponentString can be used.
  const char* getExponentString(int variable, Exponent exponent) const;
  void makeStrings() const;

  // The assigned IDs are those in the range [0, getMaxId()].
  Exponent getMaxId(int variable) const;

  // Adds a generator of the form v^e, e > 0, for any variable v where
  // generator of that form is not already present. e is chosen to be
  // larger than any exponent (i.e. ID) already present, and it maps
  // to 0. Note that this does NOT preserve order - the highest ID
  // always maps to 0. The reason for this is that this is what is
  // needed for computing irreducible decompositions.
  void addArtinianPowers(Ideal& ideal) const;

  const VarNames& getNames() const;

  // Replaces var^v by var^(a[i] - v) except that var^0 is left
  // alone. It is a precondition that a[i] be larger than or equal to
  // the mapped values.
  void dualize(const vector<mpz_class>& a);

  void print(FILE* file) const;

private:
  void clearStrings();

  TermTranslator(const TermTranslator&); // not suported
  TermTranslator& operator=(const TermTranslator&); // not supported

  void initialize(const vector<BigIdeal*>& bigIdeals, bool sortVars);
  void shrinkBigIdeal(const BigIdeal& bigIdeal, Ideal& ideal) const;
  Exponent shrinkExponent(size_t var, const mpz_class& exponent) const;

  vector<vector<mpz_class> > _exponents;
  mutable vector<vector<const char*> > _stringExponents;
  VarNames _names;
};

#endif
