#ifndef SINGULAR_IO_HANDLER_GUARD
#define SINGULAR_IO_HANDLER_GUARD

#include "IOHandler.h"

class VarNames;
class Scanner;
class BigIdeal;
class CoefTermConsumer;

class SingularIOHandler : public IOHandler {
public:
  SingularIOHandler();

  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);

 private:
  virtual void writePolynomialHeader(const VarNames& names, FILE* out);
  virtual void writeTermOfPolynomial(const mpz_class& coef,
									 const Term& term,
									 const TermTranslator* translator,
									 bool isFirst,
									 FILE* out);
  virtual void writePolynomialFooter(const VarNames& names,
									 bool wroteAnyGenerators,
									 FILE* out);

  virtual void writeIdealHeader(const VarNames& names, FILE* out);
  virtual void writeTermOfIdeal(const Term& term,
								const TermTranslator* translator,
								bool isFirst,
								FILE* out);
  virtual void writeTermOfIdeal(const vector<mpz_class> term,
								const VarNames& names,
								bool isFirst,
								FILE* out);
  virtual void writeIdealFooter(const VarNames& names,
								bool wroteAnyGenerators,
								FILE* out);

  void readVarsAndClearIdeal(BigIdeal& ideal, Scanner& scanner);
  void writeRing(const VarNames& names, FILE* out);
};

#endif
