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
#ifndef DEBUG_ALLOCATOR_GUARD
#define DEBUG_ALLOCATOR_GUARD

class TestCase;

#include <string>

#ifdef DEBUG

/** This class is reponsible for testing how Frobby recovers when
    running out of memory and encountering exceptions in general. It
    does this by running Frobby until the i'th allocation, and then
    letting that allocation fail. This is done for i=0,1,... until all
    allocation sites have been tested. For cases with many
    allocations, all allocations in the beginning and end are tested,
    and every t'th allocation in the middle is tested, for some t such
    that the running time does not explode too much.

    Note that this re-running requires rewinding the input, which is
    only possible when using a file directly as input rather than
    using standard input.

    DebugAllocator follows the Singleton Pattern, and it works by
    intercepting all allocations and deallocations. This has some
    overhead, even when DebugAllocator isn't being used, so this is
    only turned on in debug builds.

    Testing recovery from running out of memory is important, as it
    can lead not just to memory leaks, but to crashes, especially from
    destructors throwing exceptions while being destructed due to an
    exception (yes, this crashes a C++ program). Other than that, this
    is the only way to test code in this situation.

    This object accepts special debug-only command line options that
    are preceded by an underscore and placed BEFORE the action. These
    are listed below.

     -debugAlloc Test recovery from running out of memory.

     -detailAlloc Print details on what allocations are being carried
      out from where.

     -input Takes an argument that is a string indicating that input
      should be read from this file instead of standard input.

    Note that this class aids the test action by running each testcase
    individually for the purpose of testing allocation, i.e. when test
    54 is run, test 1 to 53 are not rerun in order to get to the next
    allocation site in test 54.
*/
class DebugAllocator {
 public:
  /** Runs ::frobbyMain after having processed the special debug-only
      options. Tests recovery from running out of memory if that
      option has been specified. */
  int runDebugMain(int argc, const char** argv);

  /** Allocates a buffer of size bytes. */
  void* allocate(size_t size);

  /** Allocates a bufer of size bytes. The parameters file and
      lineNumer indicate the location of the allocation site.
   */
  void* allocate(size_t size, const char* file, size_t lineNumber);

  /** Runs a test case. Tests recovery from running out of memory if
      that option has been specified.
   */
  void runTest(TestCase& test, const string& name);

  /** Returns the one and only instance of this class. */
  static DebugAllocator& getSingleton();

 private:
  /** This constructor is private to ensure that there is only one
      instance of this object as per the Singleton Pattern.
   */
  DebugAllocator();

  /** Parse and skip past debug-related options, such as _input and
      _debugAlloc. */
  void processDebugOptions(int& argc, const char**& argv);

  /** Start reading the input from the beginning again. This only
      works if an input file has been specified, not if using standard
      input.
   */
  void rewindInput();

  /** Run frobbyMain and simulate running out of memory after limit
      allocations.
  */
  void runWithLimit(int argc, const char** argv, size_t limit);

  /** Indicates whether the option has been specified to test
      recovery. Otherwise Frobby runs normally with no interference by
      this object to allocation.
   */
  bool _debugAllocation;

  /** Indicates whether to print information about allocations to
      standard error. This can help to figure out which allocation
      site has caused a problem.
  */
  bool _detailAllocation;

  /** Indicates whether a limit has been set on the number of
      allocations to allow, after which limit allocations should be
      defined.
   */
  bool _limitAllocation;

  /** Is set to true when a bad_alloc exception has been thrown by
  DebugAllocator, and so DebugAllocator should expect to see this
  exception. This helps to detect cases where bad_alloc is
  inappropriately caught and cases where bad_alloc is thrown due to
  genuinely running out of memory, and in both cases some error should
  be flagged.
  */
 bool _expectBadAllocException;

  /** Indicates whether the action being performed is test, in which
      case this object should cooperate with test so that each
      testcase is tested individually.
  */
  bool _actionIsTest;

  /** If we are limiting the number of successful allocations, this
      indicates how many allocations have been allowed (since this
      field was last reset to zero).
   */
  size_t _allocationCount;

  /** If we are limiting the number of successful allocations, this
      indicates how many allocations to allow before letting them all
      fail.
   */
  size_t _allocationLimit;

  /** The file input is gotten from. Uses standard input if this is
      the empty string.
   */
  string _inputFile;
};

#endif

#endif
