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
#include "stdinc.h"
#include "BufferPool.h"

#include <utility>
#include <limits>

BufferPool::BufferPool(size_t bufferSize):
_bufferSize(MemoryBlocks::alignThrowOnOverflow(std::max(bufferSize, sizeof(FreeNode)))),
_free(0),
_blocks() {
}

void BufferPool::growCapacity() {
  // ** Calcuate size of block (doubles capacity)
  size_t size = block().getBytesInBlock();
  if (size == 0) {
    // start out at 10 buffers
    ASSERT(block().isNull());
    if (_bufferSize > std::numeric_limits<size_t>::max() / 10)
      throw bad_alloc(); // _bufferSize * 10 overflows
    size = _bufferSize * 10;
  } else {
    // double the size
    if (size > std::numeric_limits<size_t>::max() / 2)
      throw bad_alloc(); // size * 2 overflows
    size *= 2;
  }

  // ** Allocate next block
  ASSERT(MemoryBlocks::alignNoOverflow(size) == size);
  ASSERT(size > block().getBytesInBlock());
  _blocks.allocBlock(size);
}

void BufferPool::freeAllBuffers() {
  _free = 0;
  _blocks.freeAllPreviousBlocks();
}

void BufferPool::freeAllBuffersAndBackingMemory() {
  _free = 0;
  _blocks.freeAllBlocks();
}
