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
#include "Facade.h"

Facade::Facade(bool printActions):
  _printActions(printActions)
#ifdef DEBUG
  , _doingAnAction(false)
#endif
{
}

Facade::~Facade() {
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
