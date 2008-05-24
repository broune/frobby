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
#ifndef MSM_STRATEGY_GUARD
#define MSM_STRATEGY_GUARD

#include "SliceStrategyCommon.h"
#include <string>
#include "TermConsumer.h"
#include <vector>


class MsmSlice;
class Term;
class TermTranslator;
class Projection;
class Ideal;
class TermGrader;
class IndependenceSplitter;
class SliceEvent;

class MsmStrategy : public SliceStrategyCommon, public TermConsumer {
 public:
  MsmStrategy();
  virtual ~MsmStrategy();

  // ************* new interface

  virtual Slice* setupInitialSlice(const Ideal& ideal);

  virtual void split(Slice* slice,
					 SliceEvent*& leftEvent, Slice*& leftSlice,
					 SliceEvent*& rightEvent, Slice*& rightSlice);

  void setUseIndependence(bool useIndependence) {
	_useIndependence = useIndependence;
  }

  virtual void getPivot(Term& pivot, Slice& slice); // TODO: make this private

 private:
  MsmSlice* newMsmSlice();
  virtual Slice* allocateSlice();
  virtual bool debugIsValidSlice(Slice* slice);

  void labelSplit(Slice* slice,
				  Slice*& leftSlice, Slice*& rightSlice);

  bool independenceSplit(MsmSlice* slice,
						 SliceEvent*& leftEvent, Slice*& leftSlice,
						 SliceEvent*& rightEvent, Slice*& rightSlice);

  bool _useIndependence;

  // ************* old interface
 public:

  virtual void initialize(const Slice& slice);

  // *** Methods for handling independence splits
  virtual void doingIndependenceSplit(const Slice& slice,
									  IndependenceSplitter& splitter) = 0;
  virtual void doingIndependentPart(const Projection& projection,
									bool last) = 0;
  virtual void doneWithIndependenceSplit() = 0;

  // *** Methods for handling pivot and label splits

  enum SplitType {
    LabelSplit = 1,
    PivotSplit = 2
  };
  virtual SplitType getSplitType(const Slice& slice) = 0;

  virtual size_t getLabelSplitVariable(const Slice& slice);

  // report a msm to the strategy.
  virtual void consume(const Term& term) = 0;

  // Simplifies the slice prior to a split.
  virtual void simplify(Slice& slice);


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
