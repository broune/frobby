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
#ifndef ARENA_GUARD
#define ARENA_GUARD

#include "MemoryBlocks.h"
#include <utility>

#ifdef DEBUG
#include <vector>
#endif

/** This is an arena allocator. Arena allocators are very fast at the
 cost of imposing limitations on how memory can be deallocated.

 Allocation and deallocation must occur in stack order (LIFO). In
 other words, only the most recently allocated buffer that has not
 been deallocated yet can be deallocated. It is also possible to
 deallocate all buffers that were deallocated after a given buffer. In
 DEBUG mode stack order is enforced by ASSERTs.

 Arena satisfies allocation requests out of a larger block of
 memory. When a block is exhausted another block must be allocated
 using new. This new block is at least twice the size of the previous
 block. Old blocks are never re-used though they will be deallocated
 if they become old. So the current block is replaced if and only if
 it becomes exhausted.

 The scheme of geometric block growth is used because it allows a very
 fast implementation with excellent locality of reference. This can
 consume memory beyond that which the user of the Arena needs - all
 allocators have memory overhead. Optimal performance on both speed
 and memory consumption can usully be reached by all code using the same
 Arena object when that is possible given the stack-order limitation
 on deallocation.

 All methods throw bad_alloc if backing memory allocation using new fails.
*/
class Arena {
 public:
  Arena();
  ~Arena();

  // ***** Basic void* interface *****

  /** Returns a pointer to a buffer of size bytes. Throws bad_alloc if
   that is not possible. All allocated and not freed buffers have
   unique addresses even when size is zero. */
  void* alloc(size_t size);

  /** Frees the buffer pointed to by ptr. That buffer must be the most
   recently allocated buffer from this Arena that has not yet been
   freed. Double frees are not allowed. ptr must not be null. */
  void freeTop(void* ptr);

  /** Frees the buffer pointed to by ptr and all not yet freed
   allocations that have happened since that buffer was allocated. ptr
   must not be null. */
  void freeAndAllAfter(void* ptr);

  /** Marks all previous allocations as freed. Does not deallocate
   all the backing memory. */
  void freeAllAllocs();

  /** Marks all previous allocations as freed and deallocates all
   backing memory. */
  void freeAllAllocsAndBackingMemory();

  // ***** Array interface *****

  /** As alloc(elementCount * sizeof(T)). Constructors for the
   elements of the array are not called. */
  template<class T>
  pair<T*, T*> allocArrayNoCon(size_t elementCount);

  /** As allocArrayNoCon except that constructors for the elements of
   the array are called. The constructors are called in increasing
   order of index. Constructed objects are destructed in reverse
   order if a constructor throws an exception. */
  template<class T>
  pair<T*, T*> allocArray(size_t elementCount);

  /** As freeTop(array) except that the elements of the array in the
   range (array, arrayEnd] are deconstructed in decreasing order of
   index. The destructors must not throw exceptions.

   array and arrayEnd must not be zero. */
  template<class T>
  void freeTopArray(T* array, T* arrayEnd);

  /** As freeTopArray(p.first, p.second). */
  template<class T>
  void freeTopArray(pair<T*, T*> p) {freeTopArray(p.first, p.second);}

  /** As freeAndAllAfter(array) except that the elements of the array
   in the range (array, arrayEnd] are deconstructed in decreasing
   order of index. The destructors must not throw exceptions. */
  template<class T>
  void freeArrayAndAllAfter(T* array, T* arrayEnd);

  /** As freeTopArrayAndAllAfter(p.first, p.second). */
  template<class T>
  void freeArrayAndAllAfter(pair<T*, T*> p) {
	freeArrayAndAllAfter(p.first, p.second);
  }

  // ***** Miscellaneous *****

  /** Returns true if there are no live allocations for this Arena. */
  inline bool isEmpty() const;

  /** Returns the total amount of memory allocated by this object. Includes
   excess capacity that has not been allocated by a client yet. Does NOT
   include memory for a DEBUG-only mechanism to catch bugs. */
  size_t getMemoryUsage() const {return _blocks.getMemoryUsage();}

  /** Returns an arena object that can be used for non-thread safe
   scratch memory after static objects have been initialized. The
   default contract is that each function leaves this arena with the
   exact same objects allocated as before the function was entered. It
   is fine for functions to collaborate for example by using the arena
   to return variable size objects without calling new, though care
   should be used in such cases. */
  static Arena& getArena() {return _scratchArena;}

 private:
  typedef MemoryBlocks::Block Block;
  Block& block() {return _blocks.getFrontBlock();}
  const Block& block() const {return _blocks.getFrontBlock();}

  /** Allocate a new block with at least needed bytes and at least
   double the capacity of the current block. */
  void growCapacity(size_t needed);

  /** As freeTop where ptr was allocated from an old block. */
  void freeTopFromOldBlock(void* ptr);

  /** As freeAndAllAfter where ptr was allocated from an old block. */
  void freeAndAllAfterFromOldBlock(void* ptr);

  MemoryBlocks _blocks;
  IF_DEBUG(std::vector<void*> _debugAllocs;)

  static Arena _scratchArena;
};

inline bool Arena::isEmpty() const {
  return !block().hasPreviousBlock() && block().empty();
}


inline void* Arena::alloc(size_t size) {
  // It is OK to check capacity before aligning size as capacity is aligned.
  // This single if checks for three different special circumstances:
  //  * size is 0 (size - 1 will overflow)
  //  * there is not enough capacity (size > capacity)
  //  * aligning size would cause an overflow (capacity is aligned)
  const size_t capacity = block().getBytesToRight();
  ASSERT(capacity % MemoryAlignment == 0);
  if (size - 1 >= capacity) {
	ASSERT(size == 0 || size > capacity);
	if (size == 0) {
	  size = 1;
	  if (capacity > 0)
	    goto capacityOK;
	}
	growCapacity(size);
  }
 capacityOK:
  ASSERT(0 < size);
  ASSERT(size <= block().getBytesToRight());
  ASSERT(MemoryBlocks::alignNoOverflow(size) <= block().getBytesToRight());

  char* ptr = block().position();
  block().setPosition(ptr + MemoryBlocks::alignNoOverflow(size));

  IF_DEBUG(_debugAllocs.push_back(ptr));
  return ptr;
}

inline void Arena::freeTop(void* ptr) {
  ASSERT(ptr != 0);
#ifdef DEBUG
  ASSERT(!_debugAllocs.empty());
  ASSERT(_debugAllocs.back() == ptr);
  _debugAllocs.pop_back();
#endif

  if (!block().empty())
    block().setPosition(ptr);
  else
	freeTopFromOldBlock(ptr);
}

inline void Arena::freeAndAllAfter(void* ptr) {
  ASSERT(ptr != 0);
#ifdef DEBUG
  while (!_debugAllocs.empty() && ptr != _debugAllocs.back())
	_debugAllocs.pop_back();
  ASSERT(!_debugAllocs.empty());
  ASSERT(_debugAllocs.back() == ptr);
  _debugAllocs.pop_back();
#endif

  if (block().isInBlock(ptr))
	block().setPosition(ptr);
  else
	freeAndAllAfterFromOldBlock(ptr);
}

template<class T>
pair<T*, T*> Arena::allocArrayNoCon(size_t elementCount) {
  if (elementCount > static_cast<size_t>(-1) / sizeof(T))
	throw bad_alloc();
  const size_t size = elementCount * sizeof(T);
  ASSERT(size / sizeof(T) == elementCount);
  char* buffer = static_cast<char*>(alloc(size));
  T* array = reinterpret_cast<T*>(buffer);
  T* arrayEnd = reinterpret_cast<T*>(buffer + size);
  return make_pair(array, arrayEnd);
}

template<class T>
pair<T*, T*> Arena::allocArray(size_t elementCount) {
  pair<T*, T*> p = allocArrayNoCon<T>(elementCount);
  T* it = p.first;
  try {
	for (; it != p.second; ++it) {
#undef new
	  new (it) T();
#ifdef NEW_MACRO
#define new NEW_MACRO
#endif
    }
  } catch (...) {
	freeTopArray<T>(p.first, it);
	throw;
  }
  return p;
}

template<class T>
void Arena::freeTopArray(T* array, T* arrayEnd) {
  ASSERT(array != 0);
  ASSERT(array <= arrayEnd);

  while (arrayEnd != array) {
	--arrayEnd;
	arrayEnd->~T();
  }
  freeTop(array);
}

template<class T>
void Arena::freeArrayAndAllAfter(T* array, T* arrayEnd) {
  ASSERT(array != 0);
  ASSERT(array <= arrayEnd);

  while (arrayEnd != array) {
	--arrayEnd;
	arrayEnd->~T();
  }
  freeAndAllAfter(array);
}

#endif
