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
#include "SplitStrategy.h"
#include "IndependenceSplitter.h"

class MsmSlice;
class Term;
class TermTranslator;
class Projection;
class Ideal;
class TermGrader;
class IndependenceSplitter;
class SliceEvent;
class TermConsumer;

class MsmStrategy : public SliceStrategyCommon {
 public:
  MsmStrategy(TermConsumer* consumer, const SplitStrategy* split);
  virtual ~MsmStrategy();

  virtual Slice* setupInitialSlice(const Ideal& ideal);

  virtual void split(Slice* slice,
					 SliceEvent*& leftEvent, Slice*& leftSlice,
					 SliceEvent*& rightEvent, Slice*& rightSlice);

 protected:
  virtual void getPivot(Term& pivot, Slice& slice);

 private:
  MsmSlice* newMsmSlice();
  virtual Slice* allocateSlice();
  virtual bool debugIsValidSlice(Slice* slice);

  void labelSplit(Slice* slice,
				  Slice*& leftSlice, Slice*& rightSlice);

  bool independenceSplit(MsmSlice* slice,
						 SliceEvent*& leftEvent, Slice*& leftSlice,
						 SliceEvent*& rightEvent, Slice*& rightSlice);

  size_t getLabelSplitVariable(const Slice& slice);

  IndependenceSplitter _indep;
  TermConsumer* _consumer;
};

#endif
