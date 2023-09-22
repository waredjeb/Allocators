#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>

#define MAX_BINS 7
#define MIN_BINS 3
#define GROWTH 8

class CachingAllocator {
public:
  CachingAllocator(bool debug = false) : debug_(debug) { initialiseBins(); };

  void *allocate(std::size_t size) {
    if (debug_)
      printf("Asking for %lld Bytes\n", (long long)size);

    std::size_t binIndex = findBin(size);
    if (binIndex < MAX_BINS) {
      if (!cached_blocks[binIndex].empty()) {
        const MemoryBlock cachedMemoryBlock = cached_blocks[binIndex].back();
        void *allocatedMemory = cachedMemoryBlock.ptr;
        cached_blocks[binIndex].pop_back();
        live_blocks[binIndex].push_back(cachedMemoryBlock);
        if (debug_)
          printf("Reusing block %p of size %lld Bytes\n", cachedMemoryBlock, (long long)size);
        return allocatedMemory;
      } else {
        live_blocks[binIndex].emplace_back(size);
        if (debug_)
          printf("No cached block found, creating new one %p of size %lld Bytes, binIndex %d \n", live_blocks[binIndex].back().ptr, (long long)live_blocks[binIndex].back().size, binIndex);
        return live_blocks[binIndex].back().ptr;
      }
    } else {
      throw std::bad_alloc();
    }
  }

  void deallocate(void *ptr) {
    for (size_t i = 0; i < live_blocks.size(); ++i) {
      auto bin = live_blocks[i];
      auto it = std::find_if(
          bin.begin(), bin.end(),
         [ptr](const MemoryBlock &block) { return block.ptr == ptr; });
      if (it != bin.end()) {
        if (debug_)
          printf("Deallocating block %p of size %lld to bin %lld\n", ptr,
                 (long long)it->size, (long long)i);
        const MemoryBlock block = *it;
        std::size_t binIndex = findBin(block.size);
        cached_blocks[binIndex].push_back(block);
        bin.erase(it);
        return;
      }
    }
    if (debug_)
      printf("Unable to deallocate block %p\n", ptr);
  }

private:
  struct MemoryBlock {
    MemoryBlock() = delete;
    MemoryBlock(std::size_t allocSize) {
//      ptr = new char[allocSize];
      ptr = (void*)std::malloc(allocSize);
      size = allocSize;
    }

    void *ptr;
    std::size_t size;
  };
  bool debug_;

  std::vector<std::vector<MemoryBlock>> live_blocks;
  std::vector<std::vector<MemoryBlock>> cached_blocks;

  void initialiseBins() {
    live_blocks.resize(MAX_BINS);
    cached_blocks.resize(MAX_BINS);
  }

  std::size_t roundUpToPowerOfTwo(std::size_t size) {
    std::size_t powerOfTwo = 1;
    while (powerOfTwo < size) {
      powerOfTwo *= 2;
    }
    return powerOfTwo;
  }

  std::size_t findBin(std::size_t size) {
    // Find the appropriate bin index for the given size
    return static_cast<std::size_t>(std::log(size)/std::log(GROWTH));
  };
};
