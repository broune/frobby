#include "stdinc.h"
#include "Facade.h"

Facade::Facade(bool printActions):
  _printActions(printActions) 
#ifdef DEBUG
  , _doingAnAction(false)
#endif
{
}

void Facade::printMessage(const char* message) {
  if (_printActions)
    cerr << message << flush;
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

bool Facade::isPrintingActions() const {
  return _printActions;
}
