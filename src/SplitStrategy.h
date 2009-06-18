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
class TermGrader;

/** A SplitStrategy is an implementation of a split selection strategy
 for the Slice Algorithm. Specifically, it makes a decision about what
 kind of splits to perform, and how to make any choices involved in
 performing such a split.

 Some methods can only be called on certain kinds of strategies, which
 is a violation of Liskov's Substitution principle. This is
 unfortunate, but this design makes everything work smoothly, and I
 don't see a much cleaner design.
*/
class SplitStrategy {
 public:
  virtual ~SplitStrategy();

  /** Sets pivot to the pivot of a pivot split on slice. */
  virtual void getPivot(Term& pivot, Slice& slice) const = 0;

  /** Sets pivot to the pivot of a pivot split on slice. Some pivot
   selection strategies make use of a grading.
  */
  virtual void getPivot(Term& pivot, Slice& slice, const TermGrader& grader) const = 0;

  /** Returns the variable to perform a label split on. */
  virtual size_t getLabelSplitVariable(const Slice& slice) const = 0;

  /** If returns true, only call getPivot. */
  virtual bool isPivotSplit() const = 0;

  /** If returns true, only call getLabelSplitVariable. */
  virtual bool isLabelSplit() const = 0;

  /** Returns the name of the strategy. */
  virtual const char* getName() const = 0;

  /** Returns a strategy with the given name. Returns null if no
   strategy has that name. This is the only way to create a
   SplitStrategy.
  */
  static auto_ptr<SplitStrategy> createStrategy(const string& name);

 protected:
  SplitStrategy();

  // To make these inaccessible. They are not implemented.
  SplitStrategy(const SplitStrategy&);
  SplitStrategy& operator=(const SplitStrategy&);
  bool operator==(const SplitStrategy&);
};

#endif
