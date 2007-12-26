#ifndef TERM_GRADER_GUARD
#define TERM_GRADER_GUARD

#include <vector>

class Projection;
class Term;
class TermTranslator;

class TermGrader {
public:
  TermGrader(const vector<mpz_class>& varDegrees,
	     const TermTranslator* translator);

  void getDegree(const Term& term, mpz_class& degree) const;

  void getDegree(const Term& term,
		 const Projection& projection,
		 mpz_class& degree) const;

  void getIncrementedDegree(const Term& term,
			    const Projection& projection,
			    mpz_class& degree) const;

  const mpz_class& getGrade(size_t var, Exponent exponent) const;

  Exponent getMaxExponent(size_t var) const;

  size_t getVarCount() const;

private:
  vector<vector<mpz_class> > _grades;
};

#endif
