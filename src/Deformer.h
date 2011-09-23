/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
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
#ifndef DEFORMER_GUARD
#define DEFORMER_GUARD

#include <vector>
#include <limits>

class Ideal;
class Term;
class TermPredicate;
class IdealOrderer;

/** Objects of this class encapsulate the process of applying a
 generic deformation to a monomial ideal. */
class Deformer {
 public:
  /** Apply a generic deformation to ideal such that it becomes
   generic.

   @param deformationOrder The ordering to decide how to deform like
   exponents.

   @param makeStronglyGeneric Deform to a strongly generic ideal if
   true. Otherwise deform to a weakly generic ideal.

   @todo Implement makeStronglyGeneric == false. */
  Deformer(Ideal& ideal,
           const IdealOrderer& deformationOrder,
           bool makeStronglyGeneric = true);

  /** Apply the reverse transformation on term than that applied to
   the Ideal passed to the constructor. */
  void undeform(Term& term) const;

 private:
  /** var^e undeforms to var^(_undeform[var][e]). */
  vector<vector<Exponent> > _undeform;
};

#endif
