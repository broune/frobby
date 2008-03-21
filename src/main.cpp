#include "stdinc.h"
#include "Action.h"

#include "frobby.h"

class TestTermConsumer : public Frobby::TermConsumer {
public:
  virtual ~TestTermConsumer() {}
  virtual void consume(mpz_ptr* exponentVector) {
	gmp_fprintf(stdout, " %Zd %Zd %Zd\n",
				exponentVector[0],
				exponentVector[1],
				exponentVector[2]);
  }
};

void testExternalInterface() {
  Frobby::ExternalIdeal ideal(3);
  mpz_t term[3];
  mpz_init(term[0]);
  mpz_init(term[1]);
  mpz_init(term[2]);

  int t1[3] = {2,0,0};
  for (size_t i = 0; i < 3; ++i)
	mpz_set_si(term[i], t1[i]);
  ideal.addGenerator(term);

  int t2[3] = {1,1,0};
  for (size_t i = 0; i < 3; ++i)
	mpz_set_si(term[i], t2[i]);
  ideal.addGenerator(term);

  int t3[3] = {1,0,5};
  for (size_t i = 0; i < 3; ++i)
	mpz_set_si(term[i], t3[i]);
  ideal.addGenerator(term);


  int t[3] = {10,10,10};
  mpz_ptr tt[3];
  for (size_t i = 0; i < 3; ++i) {
	mpz_set_si(term[i], t[i]);
	tt[i] = term[i];
  }

  Frobby::alexanderDual(&ideal, tt, new TestTermConsumer());
  exit(0);
}

int main(int argc, const char** argv) {
  //testExternalInterface();
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
