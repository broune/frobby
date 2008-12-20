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

#include "ctime"

Timer::Timer() {
  reset();
}

void Timer::reset() {
  _initialTime = time(0);
}

unsigned long Timer::getSeconds() const {
  return (unsigned long)(time(0) - _initialTime);
}

void Timer::print(FILE* out) const {
  unsigned long seconds = getSeconds();
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds %= 60;
  minutes %= 60;

  fputc('(', out);
  if (hours != 0)
    fprintf(out, "%luh", hours);
  if (minutes != 0 || hours != 0)
    fprintf(out, "%lum", minutes);
  fprintf(out, "%lus)", seconds);
}
