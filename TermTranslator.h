#ifndef TERM_TRANSLATOR_GUARD
#define TERM_TRANSLATOR_GUARD

#include <iterator>

class TermTranslator {
public:
  // Takes over ownership of decompressionMaps.
  TermTranslator(vector<vector<mpz_class> >* decompressionMaps):
    _decompressionMaps(decompressionMaps) {
    ASSERT(decompressionMaps != 0);
  }

  ~TermTranslator() {
    delete _decompressionMaps;
  }

  const mpz_class& getExponent(int variable, Exponent exponent) const {
    ASSERT(0 <= variable);
    ASSERT(variable < (int)_decompressionMaps->size());
    ASSERT(exponent < (*_decompressionMaps)[variable].size());

    return (*_decompressionMaps)[variable][exponent];
  }

  const mpz_class& getExponent(int variable, const ExternalTerm& term) const {
    return getExponent(variable, term[variable]);
  }

  Exponent getMaxId(int variable) const {
    ASSERT(0 <= variable);
    ASSERT(variable < (int)_decompressionMaps->size());

    return (*_decompressionMaps)[variable].size() - 1;
  }

  friend ostream& operator<<(ostream& out, const TermTranslator& translator) {
    out << "TermTranslator(" << endl;
    for (int variable = 0;
	 variable < (int)translator._decompressionMaps->size(); ++variable) {
      out << " variable " << (variable + 1) << ": ";
      copy((*(translator._decompressionMaps))[variable].begin(),
	   (*(translator._decompressionMaps))[variable].end(),
	   ostream_iterator<mpz_class>(out, " "));
      out << endl;
    }
    out << ")" << endl;

    return out;
  }

private:
  // To make it inaccessible.
  TermTranslator(const TermTranslator&);

  vector<vector<mpz_class> >* _decompressionMaps;
};

#endif
