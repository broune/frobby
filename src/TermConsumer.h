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
#ifndef TERM_CONSUMER_GUARD
#define TERM_CONSUMER_GUARD

class Term;
class VarNames;
class Ideal;

/** This class is used to transfer terms one at a time from one part of
 the program to another, and possibly to perform computations on
 those terms. These can be divided into ideals (lists of terms) and
 lists of ideals.

 Using a consumer for output, for example, allows to move the output from
 memory onto the disk without having to wait for the entire computation to
 be done, while still making it possible to store the output in memory
 in a convenient form just by using a different consumer.

 @todo: consider merging this with BigTermConsumer;
*/
class TermConsumer {
 public:
  virtual ~TermConsumer();

  /** Tell the consumer which ring is being used. */
  virtual void consumeRing(const VarNames& names);

  /** Tell the consumer that the ideals that are consumed until the
   next call to doneConsumingList are to be considered as one list of
   ideals, rather than as a number of separate ideals. The default
   implementation is to ignore this, but the consumer is free to do
   something special in this case. It is thus not in general required
   to call this method, but if you don't, make sure that every
   consumer that you want to use does what you want if you output more
   than a single ideal.
  */
  virtual void beginConsumingList();

  /** Tell the consumer to begin consuming an ideal. It is required to call
   this method before calling consume().
  */
  virtual void beginConsuming() = 0;

  /** Consume a term. */
  virtual void consume(const Term& term) = 0;

  /** Must be called once after each time beginConsuming has been
   called.
  */
  virtual void doneConsuming() = 0;

  /** Must be called once after each time beginConsumingList has been
   called.
  */
  virtual void doneConsumingList();

  /** This is a non-virtual utility method that calls the other methods
   to achieve its effect of calling beginConsuming, then consuming
   all generators, and then calling doneConsuming.
  */
  void consume(const Ideal& ideal);
};

#endif
