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
#ifndef FACADE_GUARD
#define FACADE_GUARD

#include "Timer.h"

/** This is the super class of all facades. It offers protected
    methods that are convenient for derived classes to print out what
    they are doing and how long it took, and to be able to turn this
    on and off with a boolean flag.

    @ingroup Facade

    @todo Replace this with ActionPrinter using aggregation instead of
    inheritance.
*/
class Facade {
 protected:
  /** Constructs a facade that prints out what it is doing if
      printActions is true. */
  Facade(bool printActions);
  virtual ~Facade();

  /** Prints message to standard error if printing is turned on. */
  void printMessage(const char* message);

  /** Prints message to standard error if printing is turned on, and
      records the time when the action started. endAction() must be
      called in-between two calls to beginAction.
  */
  void beginAction(const char* message);

  /** Prints to standard error the time since the last call to
      beginAction. endAction() can only be called once after each call
      to beginAction.
  */
  void endAction();

  /** Returns true if printing actions. */
  bool isPrintingActions() const;

 private:
  /** Keeps track of the time between calls to beginAction() and
      endAction().
  */
  Timer _timer;

  /** Keeps track of whether we are printing. */
  bool _printActions;

#ifdef DEBUG
  /** A debug field to assert if beginAction() is called twice without
      a call to endAction() in between, or if endAction() is called
      without a preceding call to beginAction() that has not already
      been ended.
  */
  bool _doingAnAction;
#endif
};

#endif
