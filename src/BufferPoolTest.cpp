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
#include "BufferPool.h"
#include "tests.h"

#include <list>

TEST_SUITE(BufferPool)

TEST(BufferPool, NoOp) {
  BufferPool pool1(0);
  BufferPool pool2(1);
  BufferPool pool3(100);
}

TEST(BufferPool, Grind) {
  BufferPool pool(1001);
  std::list<void*> ptrs;
  for (size_t i = 0; i < 10; ++i) {
    for (size_t j = 0; j < 100; ++j)
	  ptrs.push_back(pool.alloc());
	// free most but not all and in FIFO order
	for (size_t j = 0; j < 90; ++j) {
	  pool.free(ptrs.front());
	  ptrs.pop_front();
	}
  }
  // free rest in LIFO order
  while (!ptrs.empty()) {
	pool.free(ptrs.back());
	ptrs.pop_back();
  } 
}

TEST(BufferPool, SmallBuffers) {
  BufferPool pools[] = {1, 2, 3, 4, 5};
  for (size_t i = 0; i < sizeof(pools) / sizeof(BufferPool); ++i) {
	BufferPool& pool = pools[i];
    void* a = pool.alloc();
	pool.free(a);
    void* b = pool.alloc();
    void* c = pool.alloc();
    pool.alloc();
	pool.free(c);
	for (size_t i = 0; i < 10000; ++i)
	  pool.alloc();
    pool.free(b);
  }
}
