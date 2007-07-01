#ifndef TIMER_GUARD
#define TIMER_GUARD

class Timer {
public:
  Timer() {
    reset();
  }

  friend ostream& operator<<(ostream& out, const Timer& timer) {
    timer.print(out);
    return out;
  }

  void print(ostream& out) const {
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

  void reset() {
    _initialTime = time(0);
  }

  unsigned long getSeconds() const {
    return (unsigned long)(time(0) - _initialTime);
  }

private:
  time_t _initialTime;
};

#endif
