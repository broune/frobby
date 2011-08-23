/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef EULER_STATE_GUARD
#define EULER_STATE_GUARD

#include "RawSquareFreeIdeal.h"

class Ideal;
class Arena;

class EulerState {
public:
  static EulerState* construct(const Ideal& idealParam, Arena* arena);
  static EulerState* construct
	(const RawSquareFreeIdeal& idealParam, Arena* arena);

  EulerState* inPlaceStdSplit(size_t pivotVar);
  EulerState* inPlaceStdSplit(Word* pivot);
  EulerState* inPlaceGenSplit(size_t pivotIndex);

  bool toColonSubState(const Word* pivot);
  bool toColonSubState(size_t pivotVar);
  void toColonSubStateNoReminimizeNecessary(size_t pivotVar);
  void toColonSubStateNoReminimizeNecessary(Word* pivot);

  EulerState* makeSumSubState(size_t pivotVar);
  EulerState* makeSumSubState(Word* pivot);

  void flipSign() {sign = -sign;}
  int getSign() const {return sign;}

  void transpose();

  EulerState* getParent() {return _parent;}
  RawSquareFreeIdeal& getIdeal() {return *ideal;}
  const RawSquareFreeIdeal& getIdeal() const {return *ideal;}
  const Word* getEliminatedVars() const {return eliminated;}
  size_t getVarCount() const {return getIdeal().getVarCount();}
  size_t getNonEliminatedVarCount() const;

  void removeGenerator(size_t index) {ideal->removeGenerator(index);}

  void compactEliminatedVariablesIfProfitable();

  void print(FILE* out);

#ifdef DEBUG
  bool debugIsValid() const;
#endif

private:
  EulerState(const EulerState&); // unavailable
  static EulerState* rawConstruct(size_t varCount, size_t capacity,
								  Arena* arena);

  void toZero();

  void allocateIdealAndEliminated(size_t varCount, size_t capacity);

  void deallocate();

  RawSquareFreeIdeal* ideal;
  Word* eliminated;
  int sign;
  Arena* _alloc;
  EulerState* _parent;
};

#endif
