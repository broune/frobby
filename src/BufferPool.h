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

#include "MemoryBlocks.h"

/** Allocator for allocating and freeing same-size buffers. Uses
 a free list. All allocations are automatically freed when
 the buffer pool is destructed. */
class BufferPool {
 public:
  /** bufferSize is how many bytes are returned by each call to alloc. */
  BufferPool(size_t bufferSize);

  /** Returns a pointer to an array of getBufferSize() chars. The
   alignment is as for Arena. The lifetime of the buffer is until free
   is called with the returned value as parameter on this same object
   or clear() is called or this object is destructed.

   Do not pass the returned value to ::free, do not delete it and do
   not free it on a different BufferPool.  Throws an exception if no
   more memory can be allocated. Never returns null. */
  inline void* alloc();

  /** Makes the buffer at ptr available for reuse. ptr must be a value
   previously returned by alloc on this same object that hasn't been
   freed already since then. ptr must not be null. This method cannot
   throw an exception. */
  inline void free(void* ptr);

  /** Returns how many bytes are in each buffer. Can be more than
   requested due to having to have enough space to store a free list
   pointer in each buffer. */
  size_t getBufferSize() const {return _bufferSize;}

  /** Marks all allocated blocks as available for reuse. Does not
   deallocate the backing memory. */
  void clear();

 private:
  typedef MemoryBlocks::Block Block;
  Block& block() {return _blocks.getFrontBlock();}
  const Block& block() const {return _blocks.getFrontBlock();}

  /** Allocate another block of double the size. */
  void growCapacity();

  /** A node of the linked list of free buffers. */
  struct FreeNode {
    FreeNode* next;
  };

  const size_t _bufferSize; /// size of the buffers returned by alloc
  FreeNode* _free; /// null indicates that the free list is empty
  MemoryBlocks _blocks;
};

inline void* BufferPool::alloc() {
  if (_free != 0) {
    void* ptr = _free;
	_free = _free->next;
	return ptr;
  } else {
    if (block().position() == block().end())
      growCapacity();
    void* ptr = block().position();
    block().setPosition(block().position() + getBufferSize());
    return ptr;
  }
}

inline void BufferPool::free(void* ptr) {
  FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
  node->next = _free;
  _free = node;
}

#endif
