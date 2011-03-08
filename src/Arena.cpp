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

#include <new>

Arena::Arena() {
}

Arena::~Arena() {
  while (_block._previousBlock != 0)
	discardPreviousBlock();
  delete[] _block._blockBegin;
}

Arena::Block::Block():
  _blockBegin(0),
  _freeBegin(0),
  _blockEnd(0),
  _previousBlock(0) {
}

void Arena::growCapacity(size_t neededUnaligned) {
  const size_t minimumAlloc = 16 * 1024;
  ASSERT(minimumAlloc % MemoryAlignment == 0);

  // Align needed
  ASSERT(neededUnaligned > 0); 
  size_t needed = (neededUnaligned / MemoryAlignment) * MemoryAlignment;
  if (needed != neededUnaligned) {
	needed += MemoryAlignment;
	if (needed < neededUnaligned)
	  throw bad_alloc(); // overflow
  }

  // get at least 2x previous memory
  size_t size = std::max(minimumAlloc, _block.getSize());
  do {
	if (size != (size * 2) / 2)
	  throw bad_alloc(); // overflow
	size *= 2;
  } while (size / 2 < needed);

  // we place old block information at the end of their memory area
  if (_block._blockBegin != 0) {
	Block* previousBlock = reinterpret_cast<Block*>(_block._blockEnd);
	*previousBlock = _block;
	_block._previousBlock = previousBlock;
  }

  if (size > size + sizeof(Block)) {
	ASSERT(false); // overflow
	// This should never happen as size is a power of 2.
	throw bad_alloc();
  }

  char* buffer = new char[size + sizeof(Block)];
  _block._blockBegin = buffer;
  _block._freeBegin = buffer;
  _block._blockEnd = buffer + size;
}

void Arena::freeTopFromOldBlock(void* ptr) {
  ASSERT(ptr != 0);
  ASSERT(_block.isEmpty());
  ASSERT(_block._previousBlock != 0);

  ASSERT(_block._previousBlock->debugIsValid(ptr));
  _block._previousBlock->_freeBegin = static_cast<char*>(ptr);
  if (_block._previousBlock->isEmpty())
	discardPreviousBlock();
}

void Arena::freeAndAllAfterFromOldBlock(void* ptr) {
  ASSERT(!_block.isInBlock(ptr));
  ASSERT(_block._previousBlock != 0);

  while (!(_block._previousBlock->isInBlock(ptr))) {
	discardPreviousBlock();
	ASSERT(_block._previousBlock != 0);
  }

  ASSERT(_block._previousBlock->debugIsValid(ptr));
  _block._previousBlock->_freeBegin = static_cast<char*>(ptr);
  if (_block._previousBlock->isEmpty())
	discardPreviousBlock();
}

void Arena::discardPreviousBlock() {
  ASSERT(_block._previousBlock != 0);
  Block* before = _block._previousBlock->_previousBlock;
  delete[] _block._previousBlock->_blockBegin;
  _block._previousBlock = before;
}

bool Arena::Block::debugIsValid(const void* ptr) const {
  return _blockBegin <= ptr && ptr < _freeBegin;
}
