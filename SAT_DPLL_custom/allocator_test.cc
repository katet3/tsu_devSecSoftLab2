#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <functional>
#include <random>
#include <cassert>
#include "./lib/Allocator/Allocator.h"

struct TestBlock {
    int id;
    char data[1020];
};

Allocator stress_test_allocator_(1024, 1000);

static void HandleOutOfMemory() {
    std::cout << "Error: Out of memory" << std::endl;
    assert(false);
}

void RunAllocatorStressTest() {
    std::set_new_handler(HandleOutOfMemory);
    std::cout << "Starting allocator stress test...\n";
    
    const int kNumBlocks = 150;
    const int kBlockSize = sizeof(TestBlock);
    const int kMegabytesToAllocate = kNumBlocks * kBlockSize / (1024 * 1024);
    
    std::cout << "Attempting to allocate ~" << kMegabytesToAllocate << " MB\n";
    
    std::vector<TestBlock*> allocated_blocks_;
    int successful_allocations_ = 0;
    
    auto allocation_start_time_ = std::chrono::high_resolution_clock::now();
    
    try {
        for (int i = 0; i < kNumBlocks; ++i) {
            void* allocated_memory_ = stress_test_allocator_.Allocate(kBlockSize);
            if (!allocated_memory_) {
                std::cout << "Failed to allocate block #" << i << std::endl;
                break;
            }
            
            TestBlock* new_block_ = new(allocated_memory_) TestBlock();
            new_block_->id = i;
            
            for (int j = 0; j < 1020; ++j) {
                new_block_->data[j] = static_cast<char>(i % 256);
            }
            
            allocated_blocks_.push_back(new_block_);
            ++successful_allocations_;
            
            if (i % 10000 == 0) {
                std::cout << "Allocated blocks: " << i << " (" 
                          << (i * kBlockSize / (1024 * 1024)) << " MB)\n";
            }
        }
    } catch (const std::exception& exception) {
        std::cout << "Memory allocation exception: " << exception.what() << std::endl;
    }
    
    auto allocation_end_time_ = std::chrono::high_resolution_clock::now();
    auto allocation_duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        allocation_end_time_ - allocation_start_time_).count();
    
    std::cout << "Successfully allocated blocks: " << successful_allocations_ 
              << " out of " << kNumBlocks << std::endl;
    std::cout << "Total allocated memory: " 
              << (successful_allocations_ * kBlockSize / (1024 * 1024)) << " MB\n";
    std::cout << "Allocation time: " << allocation_duration_ << " ms\n";
    
    const int kNumChecks = std::min(100, successful_allocations_);
    int found_errors_ = 0;
    
    for (int i = 0; i < kNumChecks; ++i) {
        int random_index_ = rand() % successful_allocations_;
        TestBlock* current_block_ = allocated_blocks_[random_index_];
        
        if (current_block_->id != random_index_) {
            std::cout << "Block ID mismatch at #" << random_index_ << ": expected " 
                      << random_index_ << ", got " << current_block_->id << std::endl;
            ++found_errors_;
            continue;
        }
        
        for (int j = 0; j < 10; ++j) {
            if (current_block_->data[j] != static_cast<char>(random_index_ % 256)) {
                std::cout << "Data corruption in block #" << random_index_ 
                          << " at position " << j << std::endl;
                ++found_errors_;
                break;
            }
        }
    }
    
    std::cout << "Verified blocks: " << kNumChecks 
              << ", found errors: " << found_errors_ << std::endl;
    
    std::cout << "Starting memory deallocation...\n";
    
    auto deallocation_start_time_ = std::chrono::high_resolution_clock::now();
    
    for (TestBlock* block_ptr_ : allocated_blocks_) {
        block_ptr_->~TestBlock();
        stress_test_allocator_.Deallocate(block_ptr_);
    }
    
    auto deallocation_end_time_ = std::chrono::high_resolution_clock::now();
    auto deallocation_duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        deallocation_end_time_ - deallocation_start_time_).count();
    
    std::cout << "Deallocation time: " << deallocation_duration_ << " ms\n";
    
    std::cout << "\n===== Allocator statistics after stress test =====\n";
    std::cout << "Block size: " << stress_test_allocator_.GetBlockSize() << " bytes\n";
    std::cout << "Total blocks: " << stress_test_allocator_.GetBlockCount() << "\n";
    std::cout << "Blocks in use: " << stress_test_allocator_.GetBlocksInUse() << "\n";
    std::cout << "Allocations: " << stress_test_allocator_.GetAllocations() << "\n";
    std::cout << "Deallocations: " << stress_test_allocator_.GetDeallocations() << "\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== ALLOCATOR STRESS TEST PROGRAM ===\n\n";
    RunAllocatorStressTest();
    return 0;
}