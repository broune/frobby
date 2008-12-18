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
#ifndef FACADE_GUARD
#define FACADE_GUARD

#include "Timer.h"

class Facade {
 protected:
  Facade(bool printActions);
  virtual ~Facade();

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
