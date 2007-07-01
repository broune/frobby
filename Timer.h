#ifndef TIMER_GUARD
#define TIMER_GUARD

class Timer {
public:
  Timer();

  friend ostream& operator<<(ostream& out, const Timer& timer) {
    timer.print(out);
    return out;
  }

  void print(ostream& out) const;

  void reset();

  unsigned long getSeconds() const;

private:
  time_t _initialTime;
};

#endif
