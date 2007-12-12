#ifndef PRIMARY_DECOM_FACADE_GUARD
#define PRIMARY_DECOM_FACADE_GUARD

#include "Facade.h"

class BigIdeal;

class PrimaryDecomFacade : private Facade {
 public:
  PrimaryDecomFacade(bool printActions);

  // Clears ideal.
  void computePrimaryDecom(BigIdeal& ideal, FILE* out);
};

#endif
