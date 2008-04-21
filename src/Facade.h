#ifndef FACADE_GUARD
#define FACADE_GUARD

#include "Timer.h"

class Facade {
 protected:
  Facade(bool printActions);

  void printMessage(const char* message);
  void beginAction(const char* message);
  void endAction();

 protected:
  bool isPrintingActions() const;

 private:
  Timer _timer;
  bool _printActions;

#ifdef DEBUG
  bool _doingAnAction;
#endif
};

#endif
