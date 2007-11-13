#ifndef TERM_TRANSLATOR_GUARD
#define TERM_TRANSLATOR_GUARD

#include <iterator>

class Term;

#include "VarNames.h"
#include <vector>
#include <map>

class BigIdeal;
class Ideal;

class TermTranslator {
public:
  TermTranslator(const BigIdeal& bigIdeal);
  TermTranslator(const vector<BigIdeal*>& bigIdeals);

  // Takes over ownership of decompressionMaps.
  TermTranslator(const VarNames& names, 
		 vector<vector<mpz_class> >* decompressionMaps);
  ~TermTranslator();

  const mpz_class& getExponent(int variable, Exponent exponent) const;
  const mpz_class& getExponent(int variable, const Term& term) const;
  const char* getExponentString(int variable, Exponent exponent) const;

  Exponent getMaxId(int variable) const;

  void print(ostream& out) const;
  friend ostream& operator<<(ostream& out, const TermTranslator& translator) {
    translator.print(out);
    return out;
  }

  void shrinkBigIdeal(const BigIdeal& bigIdeal, Ideal& ideal) const;
  void addArtinianPowers(Ideal& ideal) const;

private:
  TermTranslator(const TermTranslator&); // not suported
  TermTranslator& operator=(const TermTranslator&); // not supported

  void makeStrings(const VarNames& names);

  vector<vector<mpz_class> >* _decompressionMaps;
  vector<vector<const char*> > _stringDecompressionMaps;

  vector<map<mpz_class, Exponent> > _compressionMaps;

  void initialize(const vector<BigIdeal*>& bigIdeals);
};

#endif
