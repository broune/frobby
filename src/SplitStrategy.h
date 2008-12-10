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
#ifndef SPLIT_STRATEGY_GUARD
#define SPLIT_STRATEGY_GUARD

class Term;
class Slice;
class Ideal;

class SplitStrategy {
 public:
  SplitStrategy();
  virtual ~SplitStrategy();

  virtual void getPivot(Term& pivot, Slice& slice) const = 0;
  virtual size_t getLabelSplitVariable(const Slice& slice) const = 0;

  virtual bool isPivotSplit() const = 0;
  virtual bool isLabelSplit() const = 0;
  virtual bool isFrobeniusSplit() const = 0;

  virtual const char* getName() const = 0;

  static auto_ptr<SplitStrategy> createStrategy(const string& name);

 protected:
  // To make these inaccessible.
  SplitStrategy(const SplitStrategy&);
  SplitStrategy& operator=(const SplitStrategy&);
  bool operator==(const SplitStrategy&);
};

#endif
