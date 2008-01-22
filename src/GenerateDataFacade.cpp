#include "stdinc.h"
#include "GenerateDataFacade.h"

#include "randomDataGenerators.h"

GenerateDataFacade::GenerateDataFacade(bool printActions):
  Facade(printActions) {
}

void GenerateDataFacade::generateListIdeal(BigIdeal& ideal,
										   size_t variableCount) {
  beginAction("Generating list ideal.");

  generateLinkedListIdeal(ideal, variableCount);

  endAction();
}

void GenerateDataFacade::generateIdeal(BigIdeal& ideal,
									   size_t exponentRange,
									   size_t variableCount,
									   size_t generatorCount) {
  beginAction("Generating random monomial ideal.");

  bool fullSize =
	::generateRandomIdeal(ideal, exponentRange, variableCount, generatorCount);

  if (!fullSize)
	fputs("NOTE: Generated ideal has fewer minimal "
		  "generators than requested.\n", stderr);

  endAction();  
}

void GenerateDataFacade::
generateFrobeniusInstance(vector<mpz_class>& instance) {
  beginAction("Generating random Frobenius instance.");

  ::generateRandomFrobeniusInstance(instance);

  endAction();
}
