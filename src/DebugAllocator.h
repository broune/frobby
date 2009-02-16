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

#include <string>

#ifdef DEBUG

class DebugAllocator {
 public:
  int runDebugMain(int argc, const char** argv);

  void* allocate(size_t size);
  void* allocate(size_t size, const char* file, size_t lineNumber);

  void deallocate(void* buffer);

  static DebugAllocator& getSingleton();

 private:
  DebugAllocator(); // To ensure singleton.

  void processDebugOptions(int& argc, const char**& argv);
  void rewindInput();
  void runWithLimit(int argc, const char** argv, size_t limit);

  bool _debugAllocation;
  bool _detailAllocation;
  bool _limitAllocation;
  bool _expectBadAllocException;
  size_t _allocationCount;
  size_t _allocationLimit;
  string _inputFile;
};

#endif

#endif
