/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "ActionPrinter.h"

ActionPrinter::ActionPrinter(bool printActions):
  _printActions(printActions),
  _actionBegun(false) {
}

ActionPrinter::ActionPrinter(bool printActions, const char* message):
  _printActions(printActions),
  _actionBegun(true) {
  printMessage(message);
}

ActionPrinter::~ActionPrinter() {
  if (_actionBegun)
    endAction();
}

void ActionPrinter::printMessage(const char* message) {
  if (_printActions) {
    fputs(message, stderr);
    fflush(stderr);
  }
}

void ActionPrinter::beginAction(const char* message) {
  ASSERT(!_actionBegun);
  printMessage(message);
  _actionBegun = true;
  _timer.reset();
}

void ActionPrinter::endAction() {
  ASSERT(_actionBegun);
  _actionBegun = false;
  if (_printActions) {
    fputc(' ', stderr);
    _timer.print(stderr);
    fputc('\n', stderr);
    fflush(stderr);
  }
}
