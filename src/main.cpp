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
#include "Action.h"

int main(int argc, const char** argv) {
#ifdef DEBUG
  fputs("This is a DEBUG build of Frobby. It is therefore SLOW.\n", stderr);
  fflush(stderr);
#endif
#ifdef PROFILE
  fputs("This is a PROFILE build of Frobby. It is therefore SLOW.\n", stderr);
  fflush(stderr);
#endif

  srand(time(0));

  string actionName = "help";
  if (argc > 1) {
    actionName = argv[1];
    --argc;
    ++argv;
  }

  Action* action = Action::createAction(actionName);

  if (action == 0) {
    fprintf(stderr, "ERROR: Unknown action \"%s\".\n", actionName.c_str());
    return 1;
  }

  action->parseCommandLine(argc - 1, argv + 1);
  action->perform();
  delete action;

  return 0;
}
