#include "stdinc.h"
#include "Action.h"

int main(int argc, const char** argv) {
#ifdef DEBUG
  cerr << "This is a DEBUG build of Frobby. It is therefore SLOW." << endl;
#endif
#ifdef PROFILE
  cerr << "This is a PROFILE build of Frobby. It is therefore SLOW." << endl;
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
    cerr << "ERROR: Unknown action \"" << actionName << "\"." << endl;
    return 1;
  }

  // TODO: move this into Action
  if (action->acceptsNonParameter() && argc > 1 && argv[1][0] != '-') {
    if (!action->processNonParameter(argv[1]))
      exit(1);
    --argc;
    ++argv;
  }

  action->parseCommandLine(argc - 1, argv + 1);
  action->perform();
  delete action;

  return 0;
}
