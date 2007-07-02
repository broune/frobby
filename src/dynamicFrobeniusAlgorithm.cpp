#include "stdinc.h"
#include "dynamicFrobeniusAlgorithm.h"

#include <set>

mpz_class dynamicFrobeniusAlgorithm(const vector<mpz_class>& numbers) {
  if (numbers.size() == 2)
    return numbers[0] * numbers[1] - numbers[0] - numbers[1];

  set<mpz_class> representable;
  representable.insert(0);
  mpz_class minNumber = *min_element(numbers.begin(), numbers.end());

  mpz_class maximumNotRepresentable = 0;
  int representableRun = 0;
  mpz_class number = 1;

  while (representableRun < minNumber) {
    bool isNumberRepresentable = false;

    for (size_t i = 0; i < numbers.size(); ++i) {
      if (representable.find(number - numbers[i]) !=
	  representable.end()) {
	isNumberRepresentable = true;
	break;
      }
    }

    if (isNumberRepresentable) {
      representable.insert(number);
      ++representableRun;
    } else {
      maximumNotRepresentable = number;
      representableRun = 0;
    }

    ++number;
  }

  return maximumNotRepresentable;
}
