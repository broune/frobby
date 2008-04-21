#ifndef RANDOM_DATA_GENERATORS_GUARD
#define RANDOM_DATA_GENERATORS_GUARD

#include <vector>

class BigIdeal;

void generateLinkedListIdeal(BigIdeal& ideal, unsigned int variableCount);

void generateKingChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);
void generateKnightChessIdeal(BigIdeal& ideal, unsigned int rowsAndColumns);

bool generateRandomIdeal(BigIdeal& bigIdeal,
						 unsigned int exponentRange,
						 unsigned int variableCount,
						 unsigned int generatorCount);

void generateRandomFrobeniusInstance(vector<Degree>& degrees);

#endif
