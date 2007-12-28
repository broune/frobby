#include "stdinc.h"
#include "Timer.h"

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
  fprintf(out, "%lus", seconds);
}
