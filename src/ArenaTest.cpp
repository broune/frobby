/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2011 University of Aarhus
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
#include "stdinc.h"
#include "Arena.h"
#include "tests.h"

#include <algorithm>
#include <sstream>

TEST_SUITE(Arena)

TEST(Arena, NoOp) {
  Arena arena;
}

TEST(Arena, Big) {
  Arena arena;
  void* a = arena.alloc(5);
  arena.freeTop(arena.alloc(1024 * 1024));
  arena.freeTop(arena.alloc(1024 * 1024));
  ASSERT_FALSE(arena.isEmpty());
  arena.freeTop(a);
  ASSERT_TRUE(arena.isEmpty());

}

TEST(Arena, Zero) {
  Arena arena;
  void* a = arena.alloc(0);
  void* b = arena.alloc(0);
  void* c = arena.alloc(0);
  ASSERT_NEQ(a, b);
  ASSERT_NEQ(a, c);
  ASSERT_NEQ(b, c);
  ASSERT_FALSE(arena.isEmpty());
}

TEST(Arena, Many) {
  Arena arena;
  vector<pair<char*, char*> > allocs;

  for (size_t i = 3; i < 10; ++i) {
	for (size_t size = 0; size < 100; ++size) {
	  char* a = static_cast<char*>(arena.alloc(size));
	  pair<char*, char*> p(a, a + size);
	  for (size_t j = 0; j < allocs.size(); ++j) {
		pair<char*, char*> p2 = allocs[j];
		if (p.first <= p2.first)
		  ASSERT_FALSE_SILENT(p2.first < p.second);
		else
		  ASSERT_FALSE_SILENT(p.first < p2.second);
	  }
	  fill(p.first, p.second, static_cast<char>(-1));
	  allocs.push_back(p);
	}

	while (allocs.size() > 10 * i) {
	  arena.freeTop(allocs.back().first);
	  allocs.pop_back();
	}

	arena.freeAndAllAfter(allocs[5 * i].first);
	allocs.resize(5 * i);
  }
  ASSERT_FALSE(arena.isEmpty());
  arena.freeAndAllAfter(allocs.front().first);
  ASSERT_TRUE(arena.isEmpty());
}

TEST(Arena, BigAndOverflow) {
  Arena arena;
  // aligning size causes overflow
  ASSERT_EXCEPTION(arena.alloc(static_cast<size_t>(-1)), bad_alloc);

  // 2x size is an overflow
  ASSERT_EXCEPTION(arena.alloc(static_cast<size_t>(-1)/2 + 1), bad_alloc);

  // causes attempt at allocating almost the entire virtual memory space
  // which cannot succeed
  ASSERT_EXCEPTION(arena.alloc(static_cast<size_t>(-1)/2 - 100), bad_alloc);

  // sizeof(long) * x overflows to a smaller value (0).
  const size_t smallerOverflow = 1ul << (8*sizeof(long) - 1);
  ASSERT(smallerOverflow > 0);
  ASSERT(smallerOverflow * sizeof(long) == 0);
  ASSERT_EXCEPTION(arena.allocArray<long>(smallerOverflow), bad_alloc);

  // sizeof(int) * x overflows to a greater value
  const size_t greaterOverflow = (~(0ul)) >> 1;
  ASSERT(sizeof(long) >= 4);
  ASSERT(greaterOverflow * sizeof(long) > greaterOverflow);
  ASSERT(greaterOverflow != (greaterOverflow * sizeof(long)) / sizeof(long));
  ASSERT_EXCEPTION(arena.allocArray<long>(greaterOverflow), bad_alloc);

  ASSERT_TRUE(arena.isEmpty());
}

namespace {
  template<class T, size_t ThrowAt>
  class _frobby_Helper {
  public:
	_frobby_Helper() {
	  _id = ++_count;
	  if (_id == ThrowAt) {
		_log << 'T' << _id;
		throw _id;
	  } else
		_log << '+' << _id;
	}

	~_frobby_Helper() {
	  _log << '-' << _id;
	}

	void setId(size_t id) {_id = id;}

	static string getLog() {return _log.str();}

  private:
	size_t _id;
	static size_t _count;
	static ostringstream _log;
  };

  template<class T, size_t ThrowAt>
  size_t _frobby_Helper<T, ThrowAt>::_count = 0;

  template<class T, size_t ThrowAt>
  ostringstream _frobby_Helper<T, ThrowAt>::_log;
}
#define MAKE_HELPER(NAME, THROW_AT)										\
  namespace {															\
    struct _frobby_##NAME##HelperTag {};								\
    typedef _frobby_Helper<_frobby_##NAME##HelperTag, THROW_AT> NAME##Helper; \
  }

MAKE_HELPER(ConDecon, 0)
TEST(Arena, ConDecon) {
  Arena arena;
  arena.freeTopArray(arena.allocArray<ConDeconHelper>(0));
  arena.freeTopArray(arena.allocArray<ConDeconHelper>(3));
  arena.freeTopArray(arena.allocArray<ConDeconHelper>(0));
  ASSERT_EQ(ConDeconHelper::getLog(), "+1+2+3-3-2-1");
  ASSERT_TRUE(arena.isEmpty())
}

MAKE_HELPER(ConExcep, 4)
TEST(Arena, ConExcep) {
  Arena arena;
  ASSERT_EXCEPTION(arena.allocArray<ConExcepHelper>(10), size_t);
  ASSERT_EQ(ConExcepHelper::getLog(), "+1+2+3T4-3-2-1");
  ASSERT_TRUE(arena.isEmpty());
}

MAKE_HELPER(NoConDecon, 0)
TEST(Arena, NoConDecon) {
  Arena arena;
  pair<NoConDeconHelper*, NoConDeconHelper*> p =
	arena.allocArrayNoCon<NoConDeconHelper>(3);
  p.first[0].setId(1);
  p.first[1].setId(2);
  p.first[2].setId(3);
  arena.freeTopArray(p);

  ASSERT_EQ(NoConDeconHelper::getLog(), "-3-2-1");
  ASSERT_TRUE(arena.isEmpty())
}

MAKE_HELPER(ConNoDecon, 0)
TEST(Arena, ConNoDecon) {
  Arena arena;
  arena.freeTop(arena.allocArray<ConNoDeconHelper>(3).first);
  ASSERT_EQ(ConNoDeconHelper::getLog(), "+1+2+3");
  ASSERT_TRUE(arena.isEmpty())
}
