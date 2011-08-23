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
#ifndef TIMER_GUARD
#define TIMER_GUARD

#include <ctime>

/** Measures spans of CPU time.

 The internal record of time can overflow quickly. If
 clock_t is 32 bits unsigned and CLOCKS_PER_TIC is one million
 then overflow will occur after 71 minutes. */
class Timer {
public:
  Timer() {reset();}

  /** Resets the amount of elapsed CPU time to zero. */
  void reset() {_clocksAtReset = clock();}

  /** Returns the number of CPU milliseconds since the last reset.
  See class description for time span overflow limitations. */
  unsigned long getMilliseconds() const;

  /** Prints the elapsed time in a human readable format. See
  class description for time span overflow limitations. */
  void print(FILE* out) const;

private:
  std::clock_t _clocksAtReset;
};

#endif
