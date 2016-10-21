/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * MemoryManager.h                                                                                                         *
 * ======================================================================================================================= */

#ifndef __MM__
#define __MM__

#include <cstddef>
#include "blockdata.h"
#include "dlUtils.h"

using namespace std;

class MemoryManager {
    
    public:
        MemoryManager(unsigned int memsize);
    
        void showBlockList();
    
        unsigned char *malloc(unsigned int request);
        unsigned char *calloc(unsigned int request);
        unsigned char *realloc(unsigned char *blockptr, unsigned int request);
    
        void free(unsigned char *ptr2block);
    
        template<typename Type> Type *memcopy(Type *dest_blockptr, const void *src_blockptr, size_t memsize);
        template<typename Type> Type *memset(Type *blockptr, int val, size_t memsize);

    private:
        unsigned int memsize;
        unsigned char *baseptr;
        dlNode<blockdata> *firstBlock;
    
        void splitBlock(dlNode<blockdata> *p, unsigned int chunksize);
        void mergeForward(dlNode<blockdata> *p);
        void mergeBackward(dlNode<blockdata> *p);
};


/*
 * memcopy - Copies memory of size memsize from source address to destination block pointer address (used by realloc)
 */
template<typename Type>
Type *MemoryManager::memcopy(Type *dest_blockptr, const void *src_blockptr, std::size_t memsize) {
    
    unsigned char *mem = reinterpret_cast<unsigned char *>(dest_blockptr);
    const unsigned char *src = static_cast<const unsigned char *>(src_blockptr);
    
    // Verify valid parameters
    if (mem == NULL || src == NULL || !memsize)
        return mem;
    
    typedef uint32_t dword;
    const std::size_t dwordsize = sizeof(dword), dwmsk = dwordsize - 1;
    
    std::size_t t;
    std::uintptr_t mem_uptr = reinterpret_cast<std::uintptr_t>(mem);
    std::uintptr_t src_uptr = reinterpret_cast<std::uintptr_t>(src);
    
    // Enumerations for different states of the copy
    // Determine whether to copy forward/backwards based on destination/source addresses
    enum CopyState {
        FORWARD_ALIGN,
        FORWARD_COPY_DWORDS,
        FORWARD_TRAILING_BYTES,
        BACKWARD_ALIGN,
        BACKWARD_COPY_DWORDS,
        BACKWARD_TRAILING_BYTES,
        NO_COPY = -1
    } copy = mem_uptr < src_uptr || mem_uptr > src_uptr + memsize ? FORWARD_ALIGN : mem_uptr != src_uptr ? BACKWARD_ALIGN : NO_COPY;
    
    // Determine whether destination is aligned. Set src/mem to last address if necessary
    if ((copy = copy != NO_COPY && !((src_uptr | mem_uptr) & dwmsk) ? static_cast<CopyState>(copy+1) : copy) == BACKWARD_COPY_DWORDS)
        src += memsize, mem += memsize;
    
    // Copy source to destination:
    switch (copy) {
        case FORWARD_ALIGN:             // Copy forward single bytes until memory is aligned
            for (t = memsize < dwordsize ? memsize : (src_uptr ^ mem_uptr) & dwmsk ? dwordsize - (src_uptr & dwmsk) : 0, memsize -= t; t --> 0;)
                *mem++ = *src++;
        case FORWARD_COPY_DWORDS:       // Copy forward 1 dword at a time
            for (t = memsize / dwordsize; t --> 0;) {
                *reinterpret_cast<dword *>(mem) = *reinterpret_cast<dword *>(const_cast<unsigned char *>(src));
                mem += dwordsize, src += dwordsize;
            }
        case FORWARD_TRAILING_BYTES:    // Copy any trailing bytes, if necessary
            for (t = memsize & dwmsk; t --> 0;)
                *mem++ = *src++;
            return dest_blockptr;
        case BACKWARD_ALIGN:            // Copy backwards single bytes until memory is aligned
            src += memsize, mem += memsize;
            for (t = memsize <= dwordsize ? memsize : (src_uptr ^ mem_uptr) & dwmsk ? src_uptr & dwmsk : 0, memsize -= t; t --> 0;)
                *--mem = *--src;
        case BACKWARD_COPY_DWORDS:      // Copy backwards 1 dword at a time
            for (t = memsize / dwordsize; t --> 0;) {
                src -= dwordsize, mem -= dwordsize;
                *reinterpret_cast<dword *>(mem) = *reinterpret_cast<dword *>(const_cast<unsigned char *>(src));
            }
        case BACKWARD_TRAILING_BYTES:   // Copy any trailing bytes, if necessary
            for (t = memsize & dwmsk; t --> 0;)
                *--mem = *--src;
        case NO_COPY:
            return dest_blockptr;
    }
}

/*
 * memset - Sets block memory of size memsize to specified value
 */
template<typename Type>
Type *MemoryManager::memset(Type *blockptr, int val, size_t memsize) {
    
    // Verify valid parameters
    if (blockptr == NULL || memsize == 0)
        return blockptr;
    
    // Determine initial write size for block initialization based on request size
    enum {
        _u64_ = sizeof(uint64_t),  // 64 bit / 8 byte write
        _u32_ = sizeof(uint32_t),  // 32 bit / 4 byte write
        _u16_ = sizeof(uint16_t),  // 16 bit / 2 byte write
        _u8_  = sizeof(uint8_t),   // 8 bit  / 1 byte write
    } write_size = memsize >> 3 ? _u64_ : memsize >> 2 ? _u32_ : memsize >> 1 ? _u16_ : _u8_;
    
    // Write backwards, beginning at the end of the block
    uint8_t *dst   = reinterpret_cast<uint8_t *>(blockptr + memsize);
    // Allocate 8 x 8-bit values to be used as 64-bit word
    size_t t {8};
    uint8_t *val64 = new uint8_t[t];
    
    // Fill in the array of 8-bit values with val
    for (*val64 = static_cast<uint8_t>(val); t <= write_size * 4; t *= 2) {
        *reinterpret_cast<uint64_t *>(val64) |= *reinterpret_cast<uint64_t *>(val64) << t;
    }
    
    // Alignment - align memory for 2 byte backward writes
    for (memsize -= (t = reinterpret_cast<std::uintptr_t>(dst) & (_u16_ - 1)); t --> 0;) {
        *--dst = *val64;
    }
    
    // Write word maco adjusts write destination accordingly and casts dst/val64 to the appropriate types
    #define write_word(Type, word_size) (*reinterpret_cast<Type *>((dst -= word_size)) = *reinterpret_cast<Type *>(val64))
    
    // Write chunks as large as possible until all memory has been filled
    switch (write_size) {
        case _u64_:
            for (memsize -= ((t = memsize >> 3) * _u64_); t --> 0;)
                write_word(uint64_t, _u64_);
        case _u32_:
            for (memsize -= ((t = memsize >> 2) * _u32_); t --> 0;)
                write_word(uint32_t, _u32_);
        case _u16_:
            for (memsize -= ((t = memsize >> 1) * _u16_); t --> 0;)
                write_word(uint16_t, _u16_);
        case _u8_:
            return blockptr;
    }
}

#endif
