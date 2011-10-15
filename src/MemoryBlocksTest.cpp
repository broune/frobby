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
#include "tests.h"

#include <vector>
#include <algorithm>

TEST_SUITE(MemoryBlocks)

TEST(MemoryBlocks, NoOp) {
  MemoryBlocks blocks;
}

TEST(MemoryBlocks, NoLeak) {
  MemoryBlocks blocks1;
  blocks1.allocBlock(100);
  
  MemoryBlocks blocks2;
  blocks2.allocBlock(100);
  blocks2.allocBlock(1);
  blocks2.allocBlock(0);

  // if run with a leak detector, this should pick up a leak if
  // blocks are not freed automatically.
}

TEST(MemoryBlocks, Properties) {
  MemoryBlocks blocks;
  ASSERT_TRUE(blocks.getFrontBlock().isNull());
  for (size_t i = 0; i <= 100; ++i) {
    MemoryBlocks::Block& block = blocks.allocBlock(i);
    std::fill(block.begin(), block.end(), i);
    ASSERT_EQ(&block, &blocks.getFrontBlock());
    ASSERT_FALSE(block.isNull());
    ASSERT_EQ(block.position(), block.begin());
    ASSERT_EQ(block.getBytesInBlock(), i);
    ASSERT_EQ(block.getBytesToRight(), i);
    ASSERT_EQ(block.end(), block.begin() + i);
    ASSERT_TRUE(block.empty());

    block.setPosition(block.begin() + i / 2);
    if (i > 0) {
      ASSERT_EQ(block.position(), block.begin() + i / 2);
      if (i > 1) {
      ASSERT_FALSE(block.empty());
      }
      ASSERT_TRUE(block.isInBlock(block.begin()));
      ASSERT_TRUE(block.isInBlock(block.end() - 1));
    } else {
      ASSERT_EQ(block.begin(), block.end());
    }
    ASSERT_EQ(block.getBytesToRight(), i - i / 2);
    ASSERT_FALSE(block.isInBlock(block.end()));
    ASSERT_FALSE(block.isInBlock(block.begin() - 1));

    block.clear();
    ASSERT_EQ(block.position(), block.begin());
    ASSERT_TRUE(block.empty());
  }
  for (size_t i = 100; i > 0; --i) {
    ASSERT_TRUE(blocks.getFrontBlock().hasPreviousBlock());

    MemoryBlocks::Block* previous = blocks.getFrontBlock().getPreviousBlock();
    for (const char* it = previous->begin(); it != previous->end(); ++it)
      ASSERT_EQ(*it, i - 1);
    ASSERT_EQ(previous->getBytesInBlock(), i - 1);
    blocks.freePreviousBlock();
  }
  ASSERT_FALSE(blocks.getFrontBlock().isNull());
  ASSERT_EQ(blocks.getFrontBlock().getBytesInBlock(), 100);

  blocks.allocBlock(101);
  ASSERT_EQ(blocks.getFrontBlock().getPreviousBlock()->getBytesInBlock(), 100);
  blocks.allocBlock(102);
  ASSERT_EQ(blocks.getFrontBlock().getPreviousBlock()->getBytesInBlock(), 101);

  blocks.freeAllPreviousBlocks();
  ASSERT_FALSE(blocks.getFrontBlock().isNull());
  ASSERT_FALSE(blocks.getFrontBlock().hasPreviousBlock());
  ASSERT_EQ(blocks.getFrontBlock().getBytesInBlock(), 102);

  blocks.allocBlock(103);
  blocks.freeAllBlocks();
  ASSERT_TRUE(blocks.getFrontBlock().isNull());
}
