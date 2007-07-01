#ifndef RANDOM_DATA_GENERATORS_GUARD
#define RANDOM_DATA_GENERATORS_GUARD

class BigIdeal;

bool generateRandomIdeal(BigIdeal& bigIdeal,
			 unsigned int exponentRange,
			 unsigned int variableCount,
			 unsigned int generatorCount);

// TODO: rename input to instance
void generateRandomFrobeniusInput(vector<Degree>& degrees);

#endif
