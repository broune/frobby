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
  ASSERT_TRUE(blocks.getMemoryUsage() == 0);
}

TEST(MemoryBlocks, MemoryUsage) {
  const size_t MaxOverhead =
    sizeof(MemoryBlocks::Block) + (MemoryAlignment - 1);
  MemoryBlocks blocks;
  ASSERT_EQ(blocks.getMemoryUsage(), 0);
  blocks.allocBlock(100);
  ASSERT_TRUE(blocks.getMemoryUsage() >= 100);
  ASSERT_TRUE(blocks.getMemoryUsage() <= 100 + MaxOverhead);

  blocks.allocBlock(200);
  blocks.allocBlock(400);
  ASSERT_TRUE(blocks.getMemoryUsage() >= 700);
  ASSERT_TRUE(blocks.getMemoryUsage() <= 700 + 3 * MaxOverhead);

  blocks.freeAllPreviousBlocks();
  ASSERT_TRUE(blocks.getMemoryUsage() >= 400);
  ASSERT_TRUE(blocks.getMemoryUsage() <= 400 + 3 * MaxOverhead);

  blocks.allocBlock(800);
  blocks.freeAllBlocks();
  ASSERT_EQ(blocks.getMemoryUsage(), 0);
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
    ASSERT_FALSE_SILENT(block.isNull());
    ASSERT_EQ_SILENT(block.position(), block.begin());
    ASSERT_EQ_SILENT(block.getBytesInBlock(), i);
    ASSERT_EQ_SILENT(block.getBytesToRight(), i);
    ASSERT_EQ_SILENT(block.end(), block.begin() + i);
    ASSERT_TRUE_SILENT(block.empty());

    block.setPosition(block.begin() + i / 2);
    if (i > 0) {
      ASSERT_EQ_SILENT(block.position(), block.begin() + i / 2);
      if (i > 1) {
      ASSERT_FALSE_SILENT(block.empty());
      }
      ASSERT_TRUE_SILENT(block.isInBlock(block.begin()));
      ASSERT_TRUE_SILENT(block.isInBlock(block.end() - 1));
    } else {
      ASSERT_EQ_SILENT(block.begin(), block.end());
    }
    ASSERT_EQ_SILENT(block.getBytesToRight(), i - i / 2);
    ASSERT_FALSE_SILENT(block.isInBlock(block.end()));
    ASSERT_FALSE_SILENT(block.isInBlock(block.begin() - 1));

    block.clear();
    ASSERT_EQ_SILENT(block.position(), block.begin());
    ASSERT_TRUE_SILENT(block.empty());
  }
  for (size_t i = 100; i > 0; --i) {
    ASSERT_TRUE(blocks.getFrontBlock().hasPreviousBlock());
    MemoryBlocks::Block* previous = blocks.getFrontBlock().getPreviousBlock();
    for (const char* it = previous->begin(); it != previous->end(); ++it)
      ASSERT_EQ_SILENT(*it, static_cast<char>(i - 1));
    ASSERT_EQ_SILENT(previous->getBytesInBlock(), i - 1);
    blocks.freePreviousBlock();
  }
  ASSERT_FALSE(blocks.getFrontBlock().isNull());
  ASSERT_EQ_SILENT(blocks.getFrontBlock().getBytesInBlock(), 100u);

  blocks.allocBlock(101);
  ASSERT_EQ_SILENT(blocks.getFrontBlock().getPreviousBlock()->getBytesInBlock(), 100u);
  blocks.allocBlock(102);
  ASSERT_EQ_SILENT(blocks.getFrontBlock().getPreviousBlock()->getBytesInBlock(), 101u);

  blocks.freeAllPreviousBlocks();
  ASSERT_FALSE_SILENT(blocks.getFrontBlock().isNull());
  ASSERT_FALSE_SILENT(blocks.getFrontBlock().hasPreviousBlock());
  ASSERT_EQ_SILENT(blocks.getFrontBlock().getBytesInBlock(), 102u);

  blocks.allocBlock(103);
  blocks.freeAllBlocks();
  ASSERT_TRUE(blocks.getFrontBlock().isNull());
}
