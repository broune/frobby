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
  if (_printActions) {
    fputs(message, stderr);
    fflush(stderr);
  }
}

void Facade::beginAction(const char* message) {
#ifdef DEBUG
  ASSERT(!_doingAnAction);
  _doingAnAction = true;
#endif

  if (!_printActions)
    return;

  printMessage(message);
  _timer.reset();
}

void Facade::endAction() {
#ifdef DEBUG
  ASSERT(_doingAnAction);
  _doingAnAction = false;
#endif

  if (!_printActions)
    return;

  fputc(' ', stderr);
  _timer.print(stderr);
  fputc('\n', stderr);
  fflush(stderr);
}

bool Facade::isPrintingActions() const {
  return _printActions;
}
