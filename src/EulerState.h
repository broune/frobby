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

class EulerState {
public:
  EulerState(const Ideal& idealParam);
  EulerState();
  ~EulerState();

  void inPlaceStdSplit(size_t pivotVar, EulerState& subState);
  void inPlaceStdSplit(Word* pivot, EulerState& subState);
  void inPlaceGenSplit(size_t pivotIndex, EulerState& subState);

  bool toColonSubState(const Word* pivot);
  bool toColonSubState(size_t pivotVar);
  void toColonSubStateNoReminimizeNecessary(size_t pivotVar);
  void toColonSubStateNoReminimizeNecessary(Word* pivot);
  void makeSumSubState(size_t pivotVar, EulerState& subState);
  void makeSumSubState(Word* pivot, EulerState& subState);

  void flipSign() {sign = -sign;}
  int getSign() const {return sign;}

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

  EulerState& operator=(const EulerState& state);
  void reset();

private:
  EulerState(const EulerState&); // unavailable

  void toZero();

  void allocateIdealAndEliminated(size_t varCount, size_t capacity);

  void deallocate();

  RawSquareFreeIdeal* ideal;
  Word* eliminated;
  int sign;
  size_t idealCapacity;
};

#endif
