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
#ifndef MSM_SLICE_STRATEGY_GUARD
#define MSM_SLICE_STRATEGY_GUARD

#include "SliceStrategy.h"
#include <string>
#include "TermConsumer.h"
#include <vector>

class MsmSlice;
class Term;
class TermTranslator;
class Projection;
class Ideal;
class TermGrader;

class MsmStrategy : public SliceStrategy, public TermConsumer {
 public:
  virtual ~MsmStrategy();

  // ************* new interface

  MsmSlice* setupInitialSlice(const Ideal& ideal);

  virtual pair<MsmSlice*, MsmSlice*> split(MsmSlice* slice);

  void freeSlice(MsmSlice* slice);

 private:
  MsmSlice* newSlice();

  pair<MsmSlice*, MsmSlice*> labelSplit(MsmSlice* slice);
  pair<MsmSlice*, MsmSlice*> pivotSplit(MsmSlice* slice);

  // It would make more sense with a stack, but that class has
  // (surprisingly) proven to have too high overhead.
  vector<MsmSlice*> _sliceCache;

  // ************* old interface
 public:

  virtual void initialize(const MsmSlice& slice);

  // *** Methods for handling independence splits
  virtual void doingIndependenceSplit(const MsmSlice& slice,
									  Ideal* mixedProjectionSubtract) = 0;
  virtual void doingIndependentPart(const Projection& projection,
									bool last) = 0;
  virtual bool doneWithIndependentPart() = 0;
  virtual void doneWithIndependenceSplit() = 0;

  // *** Methods for handling pivot and label splits

  enum SplitType {
    LabelSplit = 1,
    PivotSplit = 2
  };
  virtual SplitType getSplitType(const MsmSlice& slice) = 0;

  virtual void getPivot(Term& pivot, MsmSlice& slice);
  virtual size_t getLabelSplitVariable(const MsmSlice& slice);

  // report a msm to the strategy.
  virtual void consume(const Term& term) = 0;

  // Simplifies the slice prior to a split.
  virtual void simplify(MsmSlice& slice);


  // *** Static methods to create strategies.

  // These report an error and exit the program if the name is unknown.
  static MsmStrategy* newDecomStrategy(const string& name,
										 TermConsumer* consumer);
  static MsmStrategy* newFrobeniusStrategy(const string& name,
											 TermConsumer* consumer,
											 TermGrader& grader,
											 bool useBound);

  static MsmStrategy* addStatistics(MsmStrategy* strategy);
  static MsmStrategy* addDebugOutput(MsmStrategy* strategy);
};

#endif
