#include "stdinc.h"
#include "Facade.h"

Facade::Facade(bool printActions):
#ifdef DEBUG
  _doingAnAction(false),
#endif
  _printActions(printActions) {
}

void Facade::beginAction(const char* message) {
#ifdef DEBUG
  ASSERT(!_doingAnAction);
  _doingAnAction = true;
#endif

  if (!_printActions)
    return;

  cerr << message << flush;
  _timer.reset();
}

void Facade::endAction() {
#ifdef DEBUG
  ASSERT(_doingAnAction);
  _doingAnAction = false;
#endif

  if (!_printActions)
    return;

  cerr << ' ' << _timer << endl;
}
