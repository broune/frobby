#ifndef SLICE_ALGORITHM_GUARD
#define SLICE_ALGORITHM_GUARD

#include "VarNames.h"
#include <ostream>

class Ideal;
class Term;
class IOHandler;
class TermTranslator;

class SliceAlgorithm {
 public:
  SliceAlgorithm(const Ideal& ideal,
		 const VarNames& names,
		 const TermTranslator* translator,
		 ostream& out);

 private:
  void content(const Ideal& ideal,
	       const Ideal& subtract,
	       const Term& multiply);

  bool baseCase(const Ideal& ideal,
		const Ideal& subtract,
		const Term& multiply);

  void getPivot(const Ideal& ideal,
		const Ideal& subtract,
		Term& pivot);

  void simplify(Ideal& ideal,
		Term& multiply) const;

  IOHandler* _ioHandler;
  VarNames _names;
  const TermTranslator* _translator;
  ostream& _out;
};

#endif
