#include "stdinc.h"
#include "TermGrader.h"

#include "Projection.h"
#include "TermTranslator.h"
#include "Term.h"

TermGrader::TermGrader(const vector<mpz_class>& varDegrees,
		       const TermTranslator* translator):
  _grades(varDegrees.size()) {
  
  for (size_t var = 0; var < varDegrees.size(); ++var) {
    size_t maxId = translator->getMaxId(var);
    _grades[var].resize(maxId + 1);
    
    for (Exponent e = 0; e <= maxId; ++e)
      _grades[var][e] = varDegrees[var] * translator->getExponent(var, e);
  }
}

void TermGrader::getDegree(const Term& term, mpz_class& degree) const {
  ASSERT(term.getVarCount() == _grades.size());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(var, term[var]);
}

void TermGrader::getDegree(const Term& term,
			   const Projection& projection,
			   mpz_class& degree) const {
  ASSERT(term.getVarCount() == projection.getRangeVarCount());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(projection.inverseProjectVar(var), term[var]);
}

void TermGrader::getIncrementedDegree(const Term& term,
				      const Projection& projection,
				      mpz_class& degree) const {
  ASSERT(term.getVarCount() == projection.getRangeVarCount());
  degree = 0;
  for (size_t var = 0; var < term.getVarCount(); ++var)
    degree += getGrade(projection.inverseProjectVar(var), term[var] + 1);
}

const mpz_class& TermGrader::getGrade(size_t var, Exponent exponent) const {
  ASSERT(var < _grades.size());
  ASSERT(exponent < _grades[var].size());
  
  return _grades[var][exponent];
}

Exponent TermGrader::getMaxExponent(size_t var) const {
  return _grades[var].size() - 1;
}

size_t TermGrader::getVarCount() const {
  return _grades.size();
}

void TermGrader::print(FILE* file) const {
  fputs("Grader:\n", file);
  for (size_t var = 0; var < _grades.size(); ++var) {
    fprintf(file, " var %lu:", (unsigned long)var);
    for (size_t e = 0; e < _grades[var].size(); ++e)
      gmp_fprintf(file, " %lu->%Zd", (unsigned long)e,
		  _grades[var][e].get_mpz_t());
    fputc('\n', file);
  }
}
