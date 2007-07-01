#include "stdinc.h"
#include "Timer.h"

Timer::Timer() {
  reset();
}

void Timer::print(ostream& out) const {
  unsigned long seconds = getSeconds();
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds %= 60;
  minutes %= 60;

  out << '(';
  if (hours != 0)
    out << hours << 'h';
  if (minutes != 0 || hours != 0)
    out << minutes << 'm';
  out << seconds << "s)";
}

void Timer::reset() {
  _initialTime = time(0);
}

unsigned long Timer::getSeconds() const {
  return (unsigned long)(time(0) - _initialTime);
}
