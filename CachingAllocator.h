#include <iostream>
#include <vector>
#include <cmath> // For rounding up to the nearest power of 2

class CachingAllocator {
public:
    CachingAllocator() : free_list_(nullptr) {}

    void* allocate(std::size_t size) {
        // Check if the requested size exceeds the block size
        std::size_t block_size = 1;
        while (block_size < size) {
            block_size *= 2; // Round up to the nearest power of 2
        }

        // Find a block that fits the requested size
        MemoryBlock* block = findBlock(block_size);
	
        if (!block) {
            block = allocateBlock(block_size);
	    //std::cout << "Block not found for size " << size << " created new block " << block << " of size " << block->size << std::endl; 
        }else{
		//std::cout << "Found block " << block << " of size " << block->size << " that fit " << size << std::endl;
	}

        return static_cast<void*>(block);
    }

    void deallocate(void* ptr) {
        MemoryBlock* block = static_cast<MemoryBlock*>(ptr);
        block->next = free_list_;
        free_list_ = block;
    }

private:
    struct MemoryBlock {
        MemoryBlock* next;
        std::size_t size;
    };

    std::vector<char*> memory_pool_;
    MemoryBlock* free_list_;

    MemoryBlock* findBlock(std::size_t size) {
        MemoryBlock* prev = nullptr;
        MemoryBlock* current = free_list_;
        while (current) {
            if (current->size >= size) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    free_list_ = current->next;
                }
                return current;
            }
            prev = current;
            current = current->next;
        }
        return nullptr;
    }

    MemoryBlock* allocateBlock(std::size_t size) {
        //std::cout << "Allocating a new memory block of size " << size << std::endl;
        char* block = new char[size];
        memory_pool_.push_back(block);

        MemoryBlock* new_block = reinterpret_cast<MemoryBlock*>(block);
        new_block->next = nullptr;
        new_block->size = size;
        return new_block;
    }
};

