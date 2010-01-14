/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef DYNAMIC_FROBENIUS_FACADE_GUARD
#define DYNAMIC_FROBENIUS_FACADE_GUARD

#include "Facade.h"
#include <vector>

class Configuration;

/** A facade for using the dynamic programming Frobenius problem
    algorithm.

    @ingroup Facade
*/
class DynamicFrobeniusFacade : private Facade {
 public:
  DynamicFrobeniusFacade(bool printActions);

  void computeFrobeniusNumber(const vector<mpz_class>& instance,
                  mpz_class& frobeniusNumber);
};

#endif
