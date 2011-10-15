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
#include "MemoryBlocks.h"

void MemoryBlocks::freeAllPreviousBlocks() {
  while (_block.hasPreviousBlock())
	freePreviousBlock();
}

void MemoryBlocks::freeAllBlocks() {
  freeAllPreviousBlocks();
  _block.free();
  _block.makeNull();
}

void MemoryBlocks::Block::newBlock(size_t capacityInBytes) {
  if (!isNull()) {
	// We set aside space for a block at the end of the memory. Use that
	// space to store the block for the old memory.
    const size_t blockStructOffset = alignNoOverflow(getBytesInBlock());
	Block* block = reinterpret_cast<Block*>(begin() + blockStructOffset);
	block->_previous = this->getPreviousBlock();
	block->_begin = begin();
	block->_end = end();
	block->_position = position();
	_previous = block;
  }

  // make space for block information at end of new memory, but do not
  // use it yet.
  const size_t aligned = alignThrowOnOverflow(capacityInBytes);
  const size_t total = aligned + sizeof(Block);
  if (total < aligned) // check overflow
	throw std::bad_alloc();
  _begin = new char[total];
  _position = _begin;
  _end = _begin + capacityInBytes;

  ASSERT(!isNull());
  ASSERT(empty());
  ASSERT(capacityInBytes == getBytesInBlock());
}

void MemoryBlocks::Block::makeNull() {
  _previous = 0;
  _begin = 0;
  _position = 0;
  _end = 0;
}

void MemoryBlocks::Block::freePrevious() {
  ASSERT(hasPreviousBlock());
  Block* previousPrevious = getPreviousBlock()->getPreviousBlock();
  getPreviousBlock()->free();
  _previous = previousPrevious;
}
