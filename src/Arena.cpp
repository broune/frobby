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
#include <limits>

Arena Arena::_scratchArena;

Arena::Arena() {
}

Arena::~Arena() {
  clear();
}

void Arena::clear() {
  while (block().hasPreviousBlock())
	discardPreviousBlock();
  block().clear();
}

void Arena::growCapacity(const size_t needed) {
  // ** Calcuate size of block (doubles capacity)
  size_t size = std::max(needed, block().getBytesInBlock());
  if (size > std::numeric_limits<size_t>::max() / 2)
	throw bad_alloc(); // size * 2 overflows
  size *= 2;
  const size_t minimumAlloc = 16 * 1024 - sizeof(Block) - 16;
  size = std::max(size, minimumAlloc); // avoid many small blocks
  size = MemoryBlocks::alignThrowOnOverflow(size);

  ASSERT(size >= needed);
  ASSERT(size % MemoryAlignment == 0);
  _blocks.allocBlock(size);
}

void Arena::freeTopFromOldBlock(void* ptr) {
  ASSERT(ptr != 0);
  ASSERT(block().empty());
  ASSERT(block().hasPreviousBlock());

  Block* previous = block().getPreviousBlock();
  ASSERT(previous->isInBlock(ptr));
  previous->setPosition(ptr);
  if (previous->empty())
	discardPreviousBlock();
}

void Arena::freeAndAllAfterFromOldBlock(void* ptr) {
  ASSERT(!block().isInBlock(ptr));
  ASSERT(block().getPreviousBlock() != 0);

  block().setPosition(block().begin());
  while (!(block().getPreviousBlock()->isInBlock(ptr))) {
	discardPreviousBlock();
	ASSERT(block().hasPreviousBlock()); // ptr must be in some block
  }

  ASSERT(block().getPreviousBlock()->isInBlock(ptr));
  block().getPreviousBlock()->setPosition(ptr);
  if (block().getPreviousBlock()->empty())
	discardPreviousBlock();
}

void Arena::discardPreviousBlock() {
  ASSERT(block().getPreviousBlock() != 0);
  _blocks.freePreviousBlock();
}
