#ifndef TERM_TRANSLATOR_GUARD
#define TERM_TRANSLATOR_GUARD

#include <iterator>

class ExternalTerm;

class TermTranslator {
public:
  // Takes over ownership of decompressionMaps.
  TermTranslator(vector<vector<mpz_class> >* decompressionMaps);
  ~TermTranslator();

  const mpz_class& getExponent(int variable, Exponent exponent) const;
  const mpz_class& getExponent(int variable, const ExternalTerm& term) const;

  Exponent getMaxId(int variable) const;

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
  TermTranslator(const TermTranslator&); // not suported
  TermTranslator& operator=(const TermTranslator&); // not supported

  vector<vector<mpz_class> >* _decompressionMaps;
};

#endif
