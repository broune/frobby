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
  //ios_base::sync_with_stdio(false); TODO

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
