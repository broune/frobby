#ifndef TIMER_GUARD
#define TIMER_GUARD

class Timer {
public:
  Timer();

  void reset();

  unsigned long getSeconds() const;

  void print(FILE* out) const;

private:
  time_t _initialTime;
};

#endif
