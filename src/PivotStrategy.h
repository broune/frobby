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
#ifndef PIVOT_EULER_STRATEGY_GUARD
#define PIVOT_EULER_STRATEGY_GUARD

#include <vector>
#include <ostream>
#include <string>

class EulerState;
class PivotEulerAlg;

class PivotStrategy {
public:
  virtual ~PivotStrategy() {}

  virtual EulerState* doPivot(EulerState& state, const size_t* divCounts) = 0;
  virtual void getName(ostream& out) const = 0;
  virtual void computationCompleted(const PivotEulerAlg& alg) = 0;

 protected:
  PivotStrategy() {}

 private:
  PivotStrategy(const PivotStrategy&); // not available
  void operator=(const PivotStrategy&); // not available
};

auto_ptr<PivotStrategy> newDefaultPivotStrategy();

auto_ptr<PivotStrategy> newStdPivotStrategy(const string& name);
auto_ptr<PivotStrategy> newGenPivotStrategy(const string& name);
auto_ptr<PivotStrategy> newHybridPivotStrategy
  (auto_ptr<PivotStrategy> stdStrat, auto_ptr<PivotStrategy> genStrat);
auto_ptr<PivotStrategy> newDebugPivotStrategy(auto_ptr<PivotStrategy> strat,
											  FILE* out);
auto_ptr<PivotStrategy> newStatisticsPivotStrategy
(auto_ptr<PivotStrategy> strat, FILE* out);

#endif
