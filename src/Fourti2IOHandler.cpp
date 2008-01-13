#include "stdinc.h"
#include "Fourti2IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"

void Fourti2IOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  size_t termCount;
  size_t varCount;

  scanner.readInteger(termCount);
  scanner.readInteger(varCount);

  VarNames names(varCount);
  ideal.clearAndSetNames(names);

  ideal.reserve(termCount);
  for (size_t term = 0; term < termCount; ++term) {
	// Read a term
	ideal.newLastTerm();
	vector<mpz_class>& term = ideal.getLastTermRef();
	for (size_t var = 0; var < varCount; ++var)
	  scanner.readIntegerAndNegativeAsZero(term[var]);
  }
}

void Fourti2IOHandler::readIrreducibleDecomposition(Scanner& scanner,
													BigIdeal& decom) {
  fputs("ERROR: The 4ti2 format does not support decompositions.\n", stderr);
  exit(1);
}

void Fourti2IOHandler::writeIdeal(FILE* out, const BigIdeal& ideal) {
  fprintf(out, "%lu %lu\n",
		  (unsigned long)ideal.getGeneratorCount(),
		  (unsigned long)ideal.getVarCount());

  for (size_t term = 0; term < ideal.getGeneratorCount(); ++term) {
	for (size_t var = 0; var < ideal[term].size(); ++var) {
	  if (var != 0)
		fputc(' ', out);
	  gmp_fprintf(out, "%Zd", ideal[term][var].get_mpz_t());
	}
	fputc('\n', out);
  }
}

IdealWriter* Fourti2IOHandler::createWriter
(FILE* file, const VarNames& names) const {
  fputs("ERROR: The 4ti2 format does not support incremental output.", stderr);
  exit(1);
}

IdealWriter* Fourti2IOHandler::createWriter
(FILE* file, const TermTranslator* translator) const {
  fputs("ERROR: The 4ti2 format does not support incremental output.", stderr);
  exit(1);
}

const char* Fourti2IOHandler::getFormatName() const {
  return "4ti2";
}
