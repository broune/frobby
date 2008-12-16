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
#include "error.h"

#ifdef DEBUG

bool detailAllocation = false;
bool debuggingAllocation = false;
size_t goodAllocationsLeft = 0;

#undef new

void* myOperatorNew(size_t s, bool array,
					const char* file, size_t line) {
  if (detailAllocation) {
	if (file != 0)
	  fprintf(stderr, "Allocating %i bytes at %s:%i.\n",
			  (int)s, file, (int)line);
	if (file == 0)
	  fprintf(stderr, "Allocating %i bytes at an unknown point.\n",
			  (int)s);
  }

  if (debuggingAllocation && goodAllocationsLeft == 0) {
	if (detailAllocation)
	  fputs("Throwing bad_alloc due to artifically imposed limit.\n", stderr);
	throw bad_alloc();
  }

  void* p;
  if (array)
	p = malloc(s);
  else
	p = malloc(s);
  if (debuggingAllocation)
	--goodAllocationsLeft;
  return p;
}

void* operator new(size_t s) throw (std::bad_alloc) {
  return myOperatorNew(s, false, 0, 0);
}

void* operator new[](size_t s) throw (std::bad_alloc) {
  return myOperatorNew(s, true, 0, 0);
}

void operator delete(void* s) throw() {
  free(s);
}

void operator delete[](void* s) throw() {
  free(s);
}

void* operator new(size_t s, const char* file, size_t line) throw (std::bad_alloc) {
  return myOperatorNew(s, false, file, line);
}

void* operator new[](size_t s, const char* file, size_t line) throw (std::bad_alloc) {
  return myOperatorNew(s, true, file, line);
}

// Note that these overloads to operator delete are strictly
// necessary, even though they will only be called in the very special
// circumstance where an exception gets thrown while an object
// allocated using the new above has been *partially* constructed.
//
// However, when that happens, this overload gets called. The compiler
// will happily substitute its own automatically generated version,
// which does *not* deallocate the memory, leading to a memory leak.
//
// It is also important that these overloads are declared in each
// translation unit, as otherwise the compiler generated do-nothing
// version will be used in those translation units that have no such
// declaration. To me this all falls into the category of "Insane but
// true".
void operator delete(void* s, const char* file, size_t line) {
  operator delete(s);
}

void operator delete[](void* s, const char* file, size_t line) {
  operator delete[](s);
}

#endif

int normalMain(int argc, const char** argv) {
  try {
	srand((unsigned int)time(0) +
		  (unsigned int)getpid() +
		  (unsigned int)clock());

	string prefix;
	if (argc > 1) {
	  prefix = argv[1];
	  --argc;
	  ++argv;
	} else
	  prefix = "help";

	const auto_ptr<Action> action(Action::createActionWithPrefix(prefix));
	action->parseCommandLine(argc - 1, argv + 1);
	action->perform();

	return 0;
  } catch (const bad_alloc& e) {
#ifdef DEBUG
	if (debuggingAllocation && goodAllocationsLeft == 0)
	  throw;
#endif
	reportErrorNoThrow("Ran out of memory.");
	return 1;
  } catch (const InternalFrobbyException& e) {
	reportErrorNoThrow(e);
	return 2;
  } catch (const FrobbyException& e) {
	reportErrorNoThrow(e);
	return 1;
  } catch (...) {
	reportErrorNoThrow("An unexpected error occured.");
	return 1;
  }
}

// A replacement for the default C++ built-in terminate() function. Do
// not call this method or cause it to be called.
void frobbyTerminate() {
  fputs("INTERNAL ERROR: Something caused terminate() to be called. "
		"This should never happen - please contact the Frobby developers.\n",
		stderr);
  fflush(stderr);
  abort();
}

// A replacement for the default C++ built-in unexpected()
// function. Do not call this method or cause it to be called.
void frobbyUnexpected() {
  fputs("INTERNAL ERROR: Something caused unexpected() to be called. "
		"This should never happen - please contact the Frobby developers.\n",
		stderr);
  fflush(stderr);
  abort();
}

int main(int argc, const char** argv) {
  set_terminate(frobbyTerminate);
  set_unexpected(frobbyUnexpected);

#ifdef DEBUG
  fputs("This is a DEBUG build of Frobby. It is therefore SLOW.\n", stderr);
  fflush(stderr);
#endif
#ifdef PROFILE
  fputs("This is a PROFILE build of Frobby. It is therefore SLOW.\n", stderr);
  fflush(stderr);
#endif

#ifdef DEBUG
  bool optionDebugAlloc = false;
  bool optionDetailAlloc = false;
  bool optionFileInput = false;
  string inputFile;

  const char* originalArgvZero = argv[0];
  while (true) {
	if (argc <= 1 || argv[1][0] != '_')
	  break;

	string option(argv[1] + 1);
	argv += 1;
	argc -= 1;

	if (option == "debug-alloc")
	  optionDebugAlloc = true;
	else if (option == "detail-alloc")
	  optionDetailAlloc = true;
	else if (option == "input") {
	  optionFileInput = true;
	  if (argc <= 1) {
		reportErrorNoThrow("Debug option _input requries an argument.");
		return 1;
	  }
	  inputFile = argv[1];
	  argv += 1;
	  argc -= 1;
	} else {
	  reportErrorNoThrow("Unknown debug option \"" + option + "\".");
	  return 1;
	}
  }
  argv[0] = originalArgvZero;

  detailAllocation = optionDetailAlloc;

  if (optionDebugAlloc) {
	fclose(stdout);

	for (size_t goodAllocations = 0; true; ++goodAllocations) {
	  if (goodAllocations % 100 == 0)
		fprintf(stderr, "DEBUG: Trying allocation limits %i-%i.\n",
				(int)goodAllocations, (int)(goodAllocations + 99));

	  if (optionFileInput && !freopen(inputFile.c_str(), "r", stdin)) {
		reportErrorNoThrow
		  ("Could not open file \"" + inputFile + "\" for input.");
		return 1;
	  }

	  debuggingAllocation = true;
	  detailAllocation = optionDetailAlloc;
	  goodAllocationsLeft = goodAllocations;

	  try {
		int exitValue = normalMain(argc, argv);
		detailAllocation = optionDetailAlloc;
		debuggingAllocation = false;
		fprintf(stderr,
				"DEBUG: normal return from main with exit value of %i.",
				(int)exitValue);
		return exitValue;
	  } catch (bad_alloc) {
		detailAllocation = false;
		debuggingAllocation = false;
		continue;
	  }
	}
  }

  if (optionFileInput) {
	if (optionFileInput && !freopen(inputFile.c_str(), "r", stdin)) {
	  reportErrorNoThrow
		("Could not open file \"" + inputFile + "\" for input.");
	  return 1;
	}
  }
#endif

  return normalMain(argc, argv);
}
