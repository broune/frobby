/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#ifndef BIGATTI_STATE_GUARD
#define BIGATTI_STATE_GUARD

#include "Task.h"
#include "Ideal.h"
#include "Term.h"

class BigattiHilbertAlgorithm;

class BigattiState : public Task {
 public:
  BigattiState(BigattiHilbertAlgorithm* algorithm,
			   const Ideal& ideal, const Term& _multiply);

  const Ideal& getIdeal() const;
  const Term& getMultiply() const;
  size_t getVarCount() const;

  Ideal& getIdeal();
  Term& getMultiply();

  // Reorders the generators of ideal.
  Exponent getMedianPositiveExponentOf(size_t var);

  void colonStep(const Term& term);
  void colonStep(size_t var, Exponent e);
  void addStep(size_t var, Exponent e);

  virtual void run(TaskEngine& tasks);
  virtual void dispose();

 private:
  BigattiHilbertAlgorithm* _algorithm;
  Ideal _ideal;
  Term _multiply;
};

#endif
