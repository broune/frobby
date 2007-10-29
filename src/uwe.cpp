#include "stdinc.h"
#include "uwe.h"

#include "BigIdeal.h"
#include "Term.h"

// return ideal:x_{var}^\infty
void colonInf(BigIdeal& ideal, size_t var) {
  for (size_t term = 0; term < ideal.size(); ++term)
    ideal.setExponent(term, var, 0);
}

// is a a subset of b?
bool subset(BigIdeal a, BigIdeal b) {
  cerr << "calling subs." << endl;
  a.print(cerr);
  b.print(cerr);

  for (size_t termA = 0; termA < a.size(); ++termA) {
    bool included = false;
    for (size_t termB = 0; termB < b.size(); ++termB) {
      bool bDivides = true;
      for (size_t var = 0; var < a.getNames().getVarCount(); ++var) {
	if (a.getExponent(termA, var) < b.getExponent(termB, var)) {
	  bDivides = false;
	  break;
	}
      }
      if (bDivides) {
	included = true;
	break;
      }
    }
    if (!included) {
      cerr << "not sub" << endl;
      return false;
    }
  }
  cerr << "sub" << endl;
  return true;
}

void lcm(const BigIdeal& ideal, vector<mpz_class>& t) {
  t.clear();
  t.resize(ideal.getNames().getVarCount());
  for (size_t term = 0; term < ideal.size(); ++term)
    for (size_t var = 0; var < ideal.getNames().getVarCount(); ++var)
      if (ideal.getExponent(term, var) > t[var])
	t[var] = ideal.getExponent(term, var);
}

bool isZero(const BigIdeal& ideal) {
  if (ideal.size() == 0)
    return true;
  if (ideal.size() > 1)
    return false;
  for (size_t var = 0; var < ideal.getNames().getVarCount(); ++var)
    if (ideal[0][var] != 0)
      return false;
  return true;
}

bool hasArtinian(const BigIdeal& ideal, size_t v) {
  for (size_t term = 0; term < ideal.size(); ++term) {
    if (ideal.getExponent(term, v) == 0)
      continue;

    bool vArtin = true;

    for (size_t var = 0; var < ideal.getNames().getVarCount(); ++var)
      if (var != v && ideal.getExponent(term, var) != 0)
	vArtin = false;

    if (vArtin)
      return true;
  }
  return false;
}

void computePrimaryDecom(const BigIdeal& ideal,
			 vector<BigIdeal*>& ideals,
			 size_t var) {
  cerr << "Calling cpd. var=" << var << "and ideal is:" << endl;
  ideal.print(cerr);
  cerr << "------------" << endl;

  if (isZero(ideal)) {
    cerr << "was zero" << endl;
    return;
  }

  size_t varCount = ideal.getNames().getVarCount();

  if (var == varCount) {
    cerr << "!base" << endl;
    ideals.push_back(new BigIdeal(ideal));
    return;
  }

  if (hasArtinian(ideal, var)) {
    cerr << "had artin" << endl;
    computePrimaryDecom(ideal, ideals, var + 1);
    return;
  }

  vector<mpz_class> term(varCount);
  lcm(ideal, term);
  if (term[var] == 0) {
    cerr << "no var" << endl;
    computePrimaryDecom(ideal, ideals, var + 1);
    return;
  }

  BigIdeal colon(ideal);
  colonInf(colon, var);
  colon.minimize();

  BigIdeal added(ideal);
  added.newLastTerm();
  added.getLastTermExponentRef(var) = term[var];
  added.minimize();

  cerr << "calling colon" << endl;
  computePrimaryDecom(colon, ideals, var + 1);

  cerr << "calling added" << endl;
  vector<BigIdeal*> adds;
  computePrimaryDecom(added, adds, var + 1);
  for (size_t i = 0; i < adds.size(); ++i) {
    if (!subset(colon, *(adds[i])))
      ideals.push_back(adds[i]);
    else
      delete adds[i];
  }
  cerr << "returning" << endl;
}

void computePrimaryDecom(const BigIdeal& ideal,
			 vector<BigIdeal*>& ideals) {
  ideals.clear();
  computePrimaryDecom(ideal, ideals, 0);
}

void computeAssociatedPrimes(const BigIdeal& ideal,
			     BigIdeal& primes) {
  vector<BigIdeal*> primaries;
  computePrimaryDecom(ideal, primaries);

  vector<mpz_class> p;
  for (size_t i = 0; i < primaries.size(); ++i) {
    lcm(*(primaries[i]), p);
    primes.newLastTerm();
    for (size_t var = 0; var < ideal.getNames().getVarCount(); ++var)
      if (p[var] > 0)
	primes.getLastTermExponentRef(var) = 1;
  }
}
