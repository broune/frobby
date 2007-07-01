#include "stdinc.h"
#include "RandomDataFacade.h"

#include "RandomDataGenerators.h"
#include "GenerateIdealParameters.h"

RandomDataFacade::RandomDataFacade(bool printActions):
  Facade(printActions) {
}

void RandomDataFacade::generateIdeal
(BigIdeal& ideal,
 const GenerateIdealParameters& generateIdealParams) {
  beginAction("Generating random monomial ideal.");

  bool fullSize =
    ::generateRandomIdeal(ideal,
			  generateIdealParams.getExponentRange(),
			  generateIdealParams.getVariableCount(),
			  generateIdealParams.getGeneratorCount());
  endAction();

  if (!fullSize)
    cerr << "Generated ideal has fewer minimal generators than requested."
	 << endl;
}

void RandomDataFacade::generateFrobeniusInstance(vector<mpz_class>& instance) {
  beginAction("Generating random Frobenius instance.");

  ::generateRandomFrobeniusInput(instance);

  endAction();
}
