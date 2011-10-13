/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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
#ifndef OBJECT_POOL_GUARD
#define OBJECT_POOL_GUARD

#include "Arena.h"

/** Allocator for allocating and freeing same-size buffers. Uses
 a free list. All allocations are automatically freed when
 the buffer pool is destructed. */
class BufferPool {
 public:
  BufferPool(size_t bufferSize);

  void* alloc();
  void free(void* ptr);

 private:
  struct FreeNode {
    FreeNode* next;
  };

  const size_t _bufferSize;
  FreeNode* _free; // null indicates that the free list is empty
  Arena _arena;
};

#endif
