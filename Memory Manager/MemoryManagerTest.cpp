/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * MemoryManagerTest.cpp                                                                                                   *
 * ======================================================================================================================= */

#define DEBUG2 1

#if DEBUG2
#include <iostream>
#include <stdio.h>
#include "MemoryManager.h"

using namespace std;

bool testCalloc(unsigned char *blockptr, unsigned int blocksize);
bool testRealloc(unsigned char *blockptr, unsigned char *oldblockptr, unsigned int blocksize);
void showBlocks(MemoryManager *heap);

#define M "malloc"
#define C "calloc"
#define R "realloc"
#define F "free"
#define str(s) #s
#define MSG(t, x) "Doing a " str(t) " of size " str(x) "."
#define MSGF(p) "Freeing pointer p" str(p) "."

#define P_MSGF(p) (cout << MSGF(p) << endl)
#define P_MSG(t, x) (cout << MSG(t, x) << endl)

int main() {
    
    MemoryManager heaper(500);
    unsigned char *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11, *p12, *p13;

    cout << "\nHeap Initialized\n";
    showBlocks(&heaper);

    // Use malloc & calloc to allocate blocks. Print blocklist after each call
    P_MSG(M,25);
    p1 = heaper.malloc(25);
    showBlocks(&heaper);
    P_MSG(C,45);
    p2 = heaper.calloc(45);
    showBlocks(&heaper);
    P_MSG(M, 18);
    p3 = heaper.malloc(18);
    showBlocks(&heaper);
    P_MSG(C, 52);
    p4 = heaper.calloc(52);
    showBlocks(&heaper);
    P_MSG(M, 30);
    p5 = heaper.malloc(30);
    showBlocks(&heaper);
    P_MSG(C, 30);
    p6 = heaper.calloc(30);
    showBlocks(&heaper);
    P_MSG(M, 91);
    p7 = heaper.malloc(91);
    showBlocks(&heaper);
    P_MSG(C, 79);
    p8 = heaper.calloc(79);
    showBlocks(&heaper);
    
    // Verify each calloc call was successful by checking the blocks of uninitialized memory
    if (!testCalloc(p2,45)) {
        cout << "Calloc 1 failed.\n";
    }
    
    if (!testCalloc(p4,52)) {
        cout << "Calloc 2 failed.\n";
    }
    
    if (!testCalloc(p6,30)) {
        cout << "Calloc 3 failed.\n";
    }
    
    if (!testCalloc(p8,79)) {
        cout << "Calloc 4 failed.\n";
    }
    
    // Create char array
    unsigned char *mem_hold = new unsigned char[120];
    unsigned char *p_tmp = mem_hold;
    
    // Use memset to fill char array and memcopy to copy the data to the first 4 blocks we allocated
    heaper.memcopy(p1, heaper.memset(p_tmp,       'a', 25), 25);
    heaper.memcopy(p2, heaper.memset(p_tmp += 25, 'b', 45), 45);
    heaper.memcopy(p3, heaper.memset(p_tmp += 45, 'c', 18), 18);
    heaper.memcopy(p4, heaper.memset(p_tmp += 18, 'd', 32), 32);

    
    // Realloc the first 3 blocks we just copied data to
    P_MSG(R, 110);
    p9 = heaper.realloc(p1,110);
    showBlocks(&heaper);
    P_MSG(R,55);
    p10 = heaper.realloc(p2,55);
    showBlocks(&heaper);
    P_MSG(R,28);
    p11 = heaper.realloc(p3,28);
    showBlocks(&heaper);
    
    // Copy from one block to another
    heaper.memcopy(p11,p10,28);
    
    // Free block 5
    P_MSGF(5);
    heaper.free(p5);
    showBlocks(&heaper);
    
    // Realloc the 4th block and 9th block
    P_MSG(R,32);
    p12 = heaper.realloc(p4,32);
    showBlocks(&heaper);
    P_MSG(R, 89);
    p13 = heaper.realloc(p9,89);
    showBlocks(&heaper);

    // Test the first four realloc calls by verifying all data was copied properly
    if (!testRealloc(p9, mem_hold, 25)) {
        cout << "Realloc 1 failed.\n";
    }
    
    if (!testRealloc(p10, mem_hold + 25, 45)) {
        cout << "Realloc 2 failed.\n";
    }
    
    if (!testRealloc(p11, mem_hold + 25, 18)) {
        cout << "Realloc 3 failed.\n";
    }
    
    if (!testRealloc(p12, mem_hold + 88, 32))
        cout << "Realloc 4 failed.\n";

    // Free the remaining blocks
    P_MSGF(9);
    heaper.free(p9);
    showBlocks(&heaper);
    P_MSGF(3);
    heaper.free(p3);
    showBlocks(&heaper);
    P_MSGF(1);
    heaper.free(p1);
    showBlocks(&heaper);
    P_MSGF(5);
    heaper.free(p5);
    showBlocks(&heaper);
    P_MSGF(8);
    heaper.free(p8);
    showBlocks(&heaper);
    P_MSGF(11);
    heaper.free(p11);
    showBlocks(&heaper);
    P_MSGF(12);
    heaper.free(p12);
    showBlocks(&heaper);
    P_MSGF(6);
    heaper.free(p6);
    showBlocks(&heaper);
    P_MSGF(7);
    heaper.free(p7);
    showBlocks(&heaper);
    
    return 0;
}


bool testCalloc(unsigned char *blockptr, unsigned int blocksize) {
    printf("Testing calloc for pointer: %p and blocksize: %d.\n", blockptr, blocksize);
    for (unsigned char *p = blockptr; p < blockptr + blocksize; ++p) {
        if (*p) {
            cout << "Warning: found uninitialized memory at element: " << p-blockptr << ".\n\n";
            return false;
        }
    }
    cout << "All memory initialized.\n\n";
    return true;
}

bool testRealloc(unsigned char *blockptr, unsigned char *oldblockptr, unsigned int blocksize) {
    printf("Testing realloc for pointer (new): %p, pointer (old): %p and blocksize: %d.\n", blockptr, oldblockptr, blocksize);
    for (unsigned int i = 0; i < blocksize; ++i) {
        if (*blockptr++ != *oldblockptr++) {
            cout << "Warning: found non-matching data at element: " << i << ".\n";
            cout << "Element (new): " << *(blockptr-1) << " Element (old): " << *(oldblockptr-1) << ".\n\n";
            return false;
        }
    }
    cout << "All memory looks copied.\n\n";
    return true;
}

void showBlocks(MemoryManager *heap) {
    cout << "\n-------------BlockList start------------------\n";
    heap->showBlockList();
    cout << "-------------BlockList end------------------\n\n";
}

#endif