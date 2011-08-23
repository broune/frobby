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
#ifndef ACTION_PRINTER_GUARD
#define ACTION_PRINTER_GUARD

#include "Timer.h"

class ActionPrinter {
 public:
  ActionPrinter(bool printActions);
  ActionPrinter(bool printActions, const char* message);
  ~ActionPrinter();

  void printMessage(const char* message);
  void beginAction(const char* message);
  void endAction();

 private:
  const bool _printActions;
  bool _actionBegun;
  Timer _timer;
};

#endif
