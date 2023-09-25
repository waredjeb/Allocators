#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#define MAX_BINS 7
#define MIN_BINS 3
#define GROWTH 8

class CachingAllocator {
 public:
  CachingAllocator(bool debug = false) : debug_(debug) {
    initialiseBins();
    precalculateBinSizes();
  };

  void* allocate(std::size_t size) {
    if (debug_)
      printf("Asking for %lld Bytes\n", static_cast<long long>(size));

    std::size_t binIndex = findBin(size);
    if (binIndex <= MAX_BINS) {
      if (!cached_blocks[binIndex].empty()) {
        const MemoryBlock cachedMemoryBlock =
            std::move(cached_blocks[binIndex].back());
        void* allocatedMemory = cachedMemoryBlock.ptr;
        cached_blocks[binIndex].pop_back();
        live_blocks[binIndex].push_back(std::move(cachedMemoryBlock));
        if (debug_)
          printf("Reusing block %p of size %lld Bytes\n", cachedMemoryBlock.ptr,
                 static_cast<long long>(size));
        return allocatedMemory;
      } else {
        live_blocks[binIndex].emplace_back(bin_sizes[binIndex]);
        if (debug_)
          printf(
              "No cached block found, creating new one %p of size %lld Bytes, "
              "binIndex %d\n",
              live_blocks[binIndex].back().ptr,
              static_cast<long long>(live_blocks[binIndex].back().size),
              static_cast<int>(binIndex));
        return live_blocks[binIndex].back().ptr;
      }
    } else {
      throw std::bad_alloc();
    }
  }

  void deallocate(void* ptr) {
    for (size_t i = 0; i < live_blocks.size(); ++i) {
      auto& bin = live_blocks[i];
      auto it = std::find_if(
          bin.begin(), bin.end(),
          [ptr](const MemoryBlock& block) { return block.ptr == ptr; });
      if (it != bin.end()) {
        if (debug_)
          printf("Deallocating block %p of size %lld to bin %lld\n", ptr,
                 static_cast<long long>(it->size), static_cast<long long>(i));
        const MemoryBlock block = std::move(*it);
        std::size_t binIndex = findBin(block.size);
        cached_blocks[binIndex].push_back(std::move(block));
        bin.erase(it);
        return;
      }
    }
    if (debug_)
      printf("Unable to deallocate block %p\n", ptr);
  }
  // Function to release all allocated memory
  void free() {
    for (auto& bin : live_blocks) {
      for (auto& block : bin) {
        delete[] static_cast<char*>(block.ptr);
      }
    }
    for (auto& bin : cached_blocks) {
      for (auto& block : bin) {
        delete[] static_cast<char*>(block.ptr);
      }
    }
    // Clear the vectors
    live_blocks.clear();
    cached_blocks.clear();
  }

 private:
  struct MemoryBlock {
    MemoryBlock() = delete;
    MemoryBlock(std::size_t allocSize) {
      ptr = new char[allocSize];
      size = allocSize;
    }

    void* ptr;
    std::size_t size;
  };
  bool debug_;

  std::vector<std::vector<MemoryBlock>> live_blocks;
  std::vector<std::vector<MemoryBlock>> cached_blocks;
  std::vector<std::size_t> bin_sizes;

  void initialiseBins() {
    live_blocks.resize(MAX_BINS);
    cached_blocks.resize(MAX_BINS);
  }

  void precalculateBinSizes() {
    bin_sizes.reserve(MAX_BINS);
    for (std::size_t i = 0; i <= MAX_BINS; ++i) {
      bin_sizes.push_back(static_cast<std::size_t>(std::pow(GROWTH, i)));
    }
  }

  std::size_t findBin(std::size_t size) {
    // Find the appropriate bin index for the given size, rounding up
    return static_cast<std::size_t>(
        std::ceil(std::log(size) / std::log(GROWTH)));
  }
};
