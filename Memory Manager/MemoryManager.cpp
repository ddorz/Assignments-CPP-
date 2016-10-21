/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * MemoryManager.cpp                                                                                                       *
 * ======================================================================================================================= */

#include <cassert>
#include <iostream>
#include <cstdint>
#include <sstream>
#include "dlUtils.h"
#include "MemoryManager.h"

/*
 * Constructor - Allocates heap memory and initializes block data
 */
MemoryManager::MemoryManager(unsigned int memtotal): memsize(memtotal) {
    baseptr = new unsigned char[memsize];
    blockdata originalBlock(memsize, true, baseptr);
    firstBlock = new dlNode<blockdata>(originalBlock, NULL, NULL);
}

/*
 * showBlockList - Prints the current blocks
 */
void MemoryManager::showBlockList() {
    printDlList(firstBlock,"->");
}

/*
 * malloc - Searches the block list for a free block greater than or equal to request size and allocates the block
 * Priority is given to blocks with an exact amount of memory needed to fill request, to avoid splitting blocks
 * If there are no blocks of the exact size, a larger block is split and used when possible
 */
unsigned char *MemoryManager::malloc(unsigned int request) {
    
    if (request == 0)
        return NULL;
    
    dlNode<blockdata> *current, *requested_block = NULL;
    
    // Iterate through the block list and look for a blocknode to fill the request
    for (current = firstBlock; current != NULL; current = current->next) {
        if (current->info.free) {
            if (current->info.blocksize == request) {       // If free blocknode has size exactly equal to request size, break
                requested_block = NULL;
                break;                                      // If free blocknode has size larger than request, save node but keep searching
            } else if (requested_block == NULL && current->info.blocksize > request)
                requested_block = current;
        }
    }
    
    // If there are no blocknodes available to fill request, return null
    if (current == NULL && requested_block == NULL)
        return NULL;
    
    // Determine whether a blocknode of the exact size was found and split blocknode as needed
    if (current) requested_block = current;
    else splitBlock(requested_block, request);
    
    // Set the blocknode's free flag to false and return the block's address
    requested_block->info.free = false;
    return requested_block->info.blockptr;
}

/*
 * calloc - Searches the block list for a free block to fill request and allocates/initializes the block.
 * Functionality is identical to malloc except for initialization of allocated memory.
 */
unsigned char *MemoryManager::calloc(unsigned int request) {
    
    // Return NULL if request is 0
    if (request == 0)
        return NULL;
    
    dlNode<blockdata> *current, *requested_block = NULL;
    
    // Iterate through the block list and look for a blocknode to fill the request
    for (current = firstBlock; current != NULL; current = current->next) {
        if (current->info.free) {
            if (current->info.blocksize == request) {       // If free blocknode has size exactly equal to request size, break
                requested_block = NULL;
                break;                                      // If free blocknode has size larger than request, save node but keep searching
            } else if (requested_block == NULL && current->info.blocksize > request)
                requested_block = current;
        }
    }
    
    // If there are no blocknodes available to fill request, return null
    if (current == NULL && requested_block == NULL)
        return NULL;
    
    // Determine whether a blocknode of the exact size was found and split blocknode as needed
    if (current) requested_block = current;
    else splitBlock(requested_block, request);
    
    // Set the blocknode's free flag to false and return the block's address
    requested_block->info.free = false;
    
    // Determine initial write size for block initialization based on request size
    enum {
        UINT64,
        UINT32,
        UINT16,
        UINT8,
    } set_size = request >> 3 ? UINT64 : request >> 2 ? UINT32 : request >> 1 ? UINT16 : UINT8;
    
    unsigned char *ptr = requested_block->info.blockptr;
    
    // Initialize the block. Set largest possible chunks at a time. For 64/32-bit writes, align memory as necessary
    switch (set_size) {
        case UINT64:
            for (std::size_t t = reinterpret_cast<std::uintptr_t>(ptr) & 7; t > 0; --t, --request)
                *ptr++ = 0x00;
            for (size_t t = request >> 3; t > 0; --t, ptr += 8, request -= 8)
                *reinterpret_cast<uint64_t *>(ptr) = 0x00;
        case UINT32:
            for (std::size_t t = reinterpret_cast<std::uintptr_t>(ptr) & 3; t > 0; --t, --request)
                *ptr++ = 0x00;
            for (std::size_t t = request >> 2; t > 0; --t, ptr += 4, request -= 4)
                *reinterpret_cast<uint32_t *>(ptr) = 0x00;
        case UINT16:
            for (std::size_t t = request >> 1; t > 0; --t, ptr += 2, request -= 2)
                *reinterpret_cast<uint16_t *>(ptr) = 0x00;
        case UINT8:
            while (request-- > 0)
                *ptr++ = 0x00;
            break;
    }
    
    // Return block pointer
    return requested_block->info.blockptr;
}

/*
 * realloc - Attempts to reallocate the block of memory pointer to by blockptr to the request size.
 */
unsigned char *MemoryManager::realloc(unsigned char *blockptr, unsigned int request) {
    
    // Special cases
    if (blockptr == NULL) {
        return this->malloc(request);
    }
    if (request == 0) {
        this->free(blockptr);
        return NULL;
    }
    
    dlNode<blockdata> *current, *new_block = NULL;
    const unsigned char *old_blockptr = NULL;
    
    // Iterate through list and find block with matching address
    for (current = firstBlock; current!= NULL; current = current->next) {
        if (current->info.blockptr == blockptr)
            break;
    }
    
    // If the address is invalid, return NULL
    if (current == NULL)
        return NULL;
    
    // Save block size and determine the possible sizes obtained from merging adjacent free blocks
    unsigned int current_blocksize  = current->info.blocksize;
    unsigned int possible_merges[3] = {
        current_blocksize  + (current->next && current->next->info.free ? current->next->info.blocksize : 0),
        current_blocksize  + (current->prev && current->prev->info.free ? current->prev->info.blocksize : 0),
        possible_merges[0] + possible_merges[1] - current_blocksize
    };
    
    // Enum possible actions for reallocating the block and check request size against current block size
    enum ReallocAction {
        MERGE_NEXT_NO_SPLIT          =  0,
        MERGE_PREV_NO_SPLIT          =  1,
        MERGE_BOTH_NO_SPLIT          =  2,
        MERGE_NEXT                   =  3,
        MERGE_PREV                   =  4,
        MERGE_BOTH                   =  5,
        /* ~~ */
        SHRINK_MERGE_NEXT            =  6,
        SHRINK_MERGE_PREV            =  7,
        SHRINK_MERGE_BOTH            =  8,
        /* ~~ */
        NO_SUFFICIENT_MERGE          = -1,
        SHRINK_NO_MERGE              = -2,
        NO_ACTION_REQUIRED           = -3,
    } action = request < current->info.blocksize ? SHRINK_NO_MERGE : current->info.blocksize == request ? NO_ACTION_REQUIRED : NO_SUFFICIENT_MERGE;
    
    // If the request size is larger than the current block size, check the adjacent blocks for free memory.
    // Avoid using blocks that would need to be split when possible
    if (action == NO_SUFFICIENT_MERGE) {
        for (int i = 0; i < 3; ++i) {
            if (possible_merges[i] == request) {
                action = static_cast<ReallocAction>(i);
                break;
            } else if (action == NO_SUFFICIENT_MERGE && possible_merges[i] > request) {
                action = static_cast<ReallocAction>(i + 3);
            }
        } // If the request size is smaller than current block size, determine the best way to shrink the block based on adjacent free blocks
    } else if (action == SHRINK_NO_MERGE) {
        for (unsigned int i = 0, max = 0; i < 3; ++i) {
            if (possible_merges[i] > max) {
                max = possible_merges[i];
                action = static_cast<ReallocAction>(i + 6);
            }
        }
    }
    
    // Attempt to reallocate the block
    switch (action) {
            // Expand block - Merge next blocknode with current blocknode. Split next blocknode as needed
        case MERGE_NEXT:
            splitBlock(current->next, request - current->info.blocksize);
        case MERGE_NEXT_NO_SPLIT:
            mergeForward(current);
            return current->info.blockptr;
            
            // Expand block - Merge previous blocknode with current blocknode. Split current blocknode as needed
        case MERGE_PREV:
            splitBlock(current, request - current->prev->info.blocksize);
            current->next->info.free = true;
            mergeForward(current->next);
        case MERGE_PREV_NO_SPLIT:
            old_blockptr = current->info.blockptr;
            current->info.free = true;
            mergeForward((current= current->prev));
            current->info.free = false;
            return memcopy(current->info.blockptr, old_blockptr, current_blocksize);
            
            // Expand block - Merge previous and next blocknodes with current blocknode. Split next blocknode as needed
        case MERGE_BOTH:
            splitBlock(current->next, request - (current->info.blocksize + current->prev->info.blocksize));
        case MERGE_BOTH_NO_SPLIT:
            old_blockptr = current->info.blockptr;
            mergeForward(current);
            mergeBackward((current= current->prev)->next);
            return memcopy(current->info.blockptr, old_blockptr, current_blocksize);
            
            // Shrink block - Merge next block with current block. Split block to request size
        case SHRINK_MERGE_NEXT:
            mergeForward(current);
        case SHRINK_NO_MERGE:
            splitBlock(current, request);
            current->next->info.free = true;
            return current->info.blockptr;
            
            // Shrink block - Merge next and previous block with current block. Split block to request size
        case SHRINK_MERGE_BOTH:
            mergeForward(current);
        case SHRINK_MERGE_PREV:
            old_blockptr = current->info.blockptr;
            mergeBackward((current= current->prev)->next);
            splitBlock(current, request);
            current->next->info.free = true;
            return memcopy(current->info.blockptr, old_blockptr, request);
            
            // No action required - e.g. current block size == request size - return current block pointer
        case NO_ACTION_REQUIRED:
            return current->info.blockptr;
            
            // Look for new block - Request can not be filled by merging adjacent blocks. Search for a new block to fill request
        case NO_SUFFICIENT_MERGE:
            for (dlNode<blockdata> *tmp_node = firstBlock; tmp_node != NULL; tmp_node = tmp_node->next) {
                if (tmp_node->info.free) {
                    if (tmp_node->info.blocksize == request) { // Priority given to blocks that won't need to be split
                        new_block = tmp_node;
                        break;
                    } else if (new_block == NULL && tmp_node->info.blocksize > request)
                        new_block = tmp_node;
                }
            }
            if (new_block == NULL)                          // Return null if no block available to fill request
                return NULL;
            if (new_block->info.blocksize > request)        // Split new block as needed, copy data to new block, free old block
                splitBlock(new_block, request);
            old_blockptr = current->info.blockptr;
            new_block->info.free = false;
            current->info.free = true;
            mergeForward(current);
            mergeBackward(current);
            return memcopy(new_block->info.blockptr, old_blockptr, current_blocksize);
            
            // Default case - Should never execute
        default: abort();
    }
}

/*
 * free - Attempts to deallocate the block of memory with the block pointer matching ptr2block
 */
void MemoryManager::free(unsigned char *ptr2block) {
    
    // Iterate through list and check each block pointer for block to be freed
    for (dlNode<blockdata> *current = firstBlock; current != NULL; current = current->next) {
        // When the block is located, set the free flag to true and check for consecutive free blocks using mergeForward/mergeBackward
        if (current->info.blockptr == ptr2block) {
            current->info.free = true;
            mergeForward(current);
            mergeBackward(current);
            return;
        }
    }
}

/*
 * splitBlock - Split the specified free block of memory into another block of the requested size
 */
void MemoryManager::splitBlock(dlNode<blockdata> *p, unsigned int chunksize) {
    
    // Verify the block to be split (p) and split size (chunksize) are valid
    assert(p && p->info.blocksize - chunksize > 0);
    
    // Split the current blocknode to chunksize by creating another blocknode and inserting it into blocklist
    blockdata newBlock(p->info.blocksize - chunksize, p->info.free, p->info.blockptr + chunksize);
    
    // Allocate the new blocknode using the above blockdata
    dlNode<blockdata> *new_node = new dlNode<blockdata>(newBlock, p, p->next);
    
    // Insert the new blocknode into the blocklist directly after the current node (p)
    if (new_node->next) new_node->next->prev = new_node;
    p->next = new_node;
    
    // Set the current blocknode's block size to chunksize
    p->info.blocksize = chunksize;
}

/*
 * mergeForward - Checks the blocknode proceeding p and merges it with p if the memory is free
 */
void MemoryManager::mergeForward(dlNode<blockdata> *p) {
    
    //If the next block is free, merge the next block with the current block
    if (p->next && p->next->info.free == true) {
        
        // Add the next block's memory to current block
        p->info.blocksize += p->next->info.blocksize;
        
        // Link the node proceeding the next blocknode to the current blocknode
        if (p->next->next) p->next->next->prev = p;
        
        // Save pointer to next blocknode (to be removed)
        dlNode<blockdata> *remove_node = p->next;
        
        // Link the list together without the blocknode we've merged
        p->next = p->next->next;
        
        // Delete the unlinked node
        delete remove_node;
    }
}

/*
 * mergeBackward - Checks the blocknode preceding p and merges p with the preceding blocknode if the memory is free
 */
void MemoryManager::mergeBackward(dlNode<blockdata> *p) {
    
    // If the previous block is free, merge the current block with the previous block
    if (p->prev && p->prev->info.free == true) {
        
        // Add current block's size to previous block's size
        p->prev->info.blocksize += p->info.blocksize;
        p->prev->info.free = p->info.free;
        
        // Unlink the current blocknode from block list
        p->prev->next = p->next;
        if (p->next) p->next->prev = p->prev;
        
        // Delete the unlinked node
        delete p;
    }
}