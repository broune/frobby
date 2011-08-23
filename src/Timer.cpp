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
#include "stdinc.h"
#include "Timer.h"

unsigned long Timer::getMilliseconds() const {
  const double floatSpan = clock() - _clocksAtReset;
  const double floatMilliseconds = 1000 * (floatSpan / CLOCKS_PER_SEC);
  unsigned long milliseconds = static_cast<unsigned long>(floatMilliseconds);
  if (floatMilliseconds - milliseconds >= 0.5)
    ++milliseconds;
  return milliseconds;
}

void Timer::print(FILE* out) const {
  unsigned long milliseconds = getMilliseconds();
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  milliseconds %= 1000;
  seconds %= 60;
  minutes %= 60;

  fputc('(', out);
  if (hours != 0)
    fprintf(out, "%luh", hours);
  if (minutes != 0 || hours != 0)
    fprintf(out, "%lum", minutes);
  fprintf(out, "%lu.%03lus)", seconds, milliseconds);
}
