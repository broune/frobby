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
  ios_base::sync_with_stdio(false);

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

  action->parseCommandLine(argc - 1, argv + 1);
  action->perform();
  delete action;

  return 0;
}
