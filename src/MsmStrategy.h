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
  MsmStrategy(TermConsumer* consumer, const SplitStrategy* splitStrategy);

  virtual auto_ptr<Slice> beginComputing(const Ideal& ideal);
  virtual void doneComputing();

  virtual void split(auto_ptr<Slice> slice,
					 SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
					 SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice);

 protected:
  virtual void getPivot(Term& pivot, Slice& slice);
  virtual void getPivot(Term& pivot, Slice& slice, const TermGrader& grader);

 private:
  auto_ptr<MsmSlice> newMsmSlice();
  virtual auto_ptr<Slice> allocateSlice();
  virtual bool debugIsValidSlice(Slice* slice);

  void labelSplit(auto_ptr<MsmSlice> slice,
				  auto_ptr<Slice>& leftSlice, auto_ptr<Slice>& rightSlice);

  void independenceSplit(auto_ptr<MsmSlice> slice,
						 SliceEvent*& leftEvent, auto_ptr<Slice>& leftSlice,
						 SliceEvent*& rightEvent, auto_ptr<Slice>& rightSlice);

  size_t getLabelSplitVariable(const Slice& slice);

  IndependenceSplitter _indep;
  TermConsumer* _consumer;
};

#endif
