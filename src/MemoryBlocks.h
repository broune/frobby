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
#ifndef MEMORY_BLOCKS_GUARD
#define MEMORY_BLOCKS_GUARD

/** Handles a linked list of blocks of memory. Intended for use in
 implementing other memory allocators.  */
class MemoryBlocks {
 public:
   class Block;
   
   /** Makes the front block a null block object. No memory is
   allocated. */
  MemoryBlocks() {}

  /** Makes the front block have the given capacity. */
  MemoryBlocks(size_t capacityInBytes) {allocBlock(capacityInBytes);}

  /** Frees all blocks. */
  ~MemoryBlocks() {freeAllBlocks();}

  /** Create a new front block with the given capacity. If the
   previous front block is not null, it becomes the previous block of
   the new front block. This invalidates all block pointers. */
  Block& allocBlock(size_t capacityInBytes);

  /** Frees the block previous to the front block. The block before
   that, if any, becomes the new front block. There must be a previous
   block to call this method. */
  void freePreviousBlock() {_block.freePrevious();}

  /** Frees all blocks except the front block which is not changed. */
  void freeAllPreviousBlocks();

  /** Frees all blocks including the front block. The new front block
   will be a null block object. */
  void freeAllBlocks();

  /** Returns the current front block. Can be a null block object. */
  Block& getFrontBlock() {return _block;}

  /** Returns the current front block. Can be a null block object. */
  Block const& getFrontBlock() const {return _block;}

  /** Returns the total amount of memory allocated by this object. */
  size_t getMemoryUsage() const;

  /** Rounds value up to the nearest multiple of MemoryAlignment. This
   rounded up value must be representable in a size_t. */
  inline static size_t alignNoOverflow(size_t value);

  /** Rounds value up to the nearest multiple of
   MemoryAlignment. Throw std::bad_alloc if this rounded up value is
   not representable in a size_t. */
  inline static size_t alignThrowOnOverflow(size_t value);

  /** A block owns a range of memory [begin(), end()) and keeps track
   of a position within the range [begin(), end()] given by
   position(). It is possible for a block to own no memory, in which
   case begin(), end() and position are null - we call this a null
   block object. Do not create your own blocks - their lifetime is
   handled by MemoryBlocks. */
  class Block {
  public:
	char* begin() {return _begin;}
	char* const begin() const {return _begin;}
	char* end() {return _end;}
	char* const end() const {return _end;}
    char* position() {return _position;}
    char const* position() const {return _position;}
	inline void setPosition(const void* position);

	/** Returns true if ptr is in the range [begin(), end()). */
	inline bool isInBlock(const void* ptr) const;

	/** Returns the number of bytes in the range [begin(), end()). */
	size_t getBytesInBlock() const {return _end - _begin;}

	/** Returns the number of bytes in the range [position(), end()). */
	size_t getBytesToRight() const {return _end - _position;}

	/** Returns true if position() == begin(). */
	bool empty() const {return position() == begin();}

    Block* getPreviousBlock() {return _previous;}
    Block const* getPreviousBlock() const {return _previous;}

	/** Returns true if previous() is not null. */
	bool hasPreviousBlock() const {return _previous != 0;}

	/** Returns true this is a null block object. That is, if begin(),
		end() and position() are all null. */
	bool isNull() const {return begin() == 0;}

	/** Sets position to begin(). */
    void clear() {setPosition(begin());}

  private:
    friend class MemoryBlocks;
	Block() {makeNull();} 
    Block(size_t capacity, Block* previous);
	Block(Block const&); // unavailable
	void operator=(Block const&); // unavailable

	/** Makes this a null block object. Does NOT free the owned memory! */
	void makeNull();

	/** Frees the memory for this block. */
	void free() {delete[] begin();}

	/** Frees the memory for the previous block. */
	void freePrevious();

	/** Makes new memory for this block and puts the old memory in a
		block previous to this block. */
	void newBlock(size_t capacity);

	char* _begin; /// beginning of current block (aligned)
	char* _position; /// pointer to first free byte (aligned)
	char* _end; /// one past last byte (aligned)
	Block* _previous; /// null if no previous block
  };

 private:
  Block _block;
};

inline MemoryBlocks::Block& MemoryBlocks::allocBlock(size_t capacityInBytes) {
  _block.newBlock(capacityInBytes);
  return _block;
}

inline void MemoryBlocks::Block::setPosition(void const* position) {
  ASSERT(position == end() || isInBlock(position));
  _position = const_cast<char*>(reinterpret_cast<char const*>(position));
}

inline size_t MemoryBlocks::alignNoOverflow(const size_t value) {
  // this function might look big, but the total compiled code size is
  // one addition and one bitwise and.
  const size_t decAlign = MemoryAlignment - 1; // compile time constant

  ASSERT((MemoryAlignment & (decAlign)) == 0) // power of 2
  // This works because MemoryAlignment is a power of 2.
  const size_t aligned = (value + decAlign) & (~decAlign);

  ASSERT(aligned % MemoryAlignment == 0); // alignment
  ASSERT(aligned >= value); // no overflow
  ASSERT(aligned - value < MemoryAlignment); // adjustment minimal
  return aligned;
}

inline size_t MemoryBlocks::alignThrowOnOverflow(size_t value) {
  // this function might look big, but the total compiled code size is
  // one addition, one branch using a comparison and one bitwise and.
  const size_t decAlign = MemoryAlignment - 1; // compile time constant

  ASSERT((MemoryAlignment & (decAlign)) == 0) // power of 2
  // This sum overflows if and only if rounding up overflows because
  // MemoryAlignment is a power of 2.
  const size_t sum = value + decAlign;
  if (sum < value)
	throw std::bad_alloc(); // overflow
  const size_t aligned = sum & (~decAlign);

  ASSERT(aligned % MemoryAlignment == 0); // alignment
  ASSERT(aligned >= value); // no overflow
  ASSERT(aligned - value < MemoryAlignment); // adjustment minimal
  return aligned;
}

inline bool MemoryBlocks::Block::isInBlock(const void* ptr) const {
  // We use a trick to check this using one branch and two subtractions
  // instead of two branches.
  const char* p = static_cast<const char*>(ptr);
  const size_t offset = static_cast<size_t>(p - begin());
  // if _blockBegin > ptr then offset overflows to a large integer
  ASSERT((offset < getBytesInBlock()) == (begin() <= p && p < end()));
  return offset < getBytesInBlock();
}

#endif
