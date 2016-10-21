/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * blockdata.h                                                                                                             *
 * ======================================================================================================================= */

#ifndef _BLOCKDATA_
#define _BLOCKDATA_

#include <iostream>
#include "dlUtils.h"
#include "blockdata.h"

using namespace std;

class blockdata {
    
    friend ostream& operator << (ostream &out, const blockdata &B) {
        return (out << "[" << B.blocksize << (B.free ? ",free" : ",allocated") << "]");
    }
    
    public:
        blockdata(int s, bool f, unsigned char *p): blocksize(s), free(f), blockptr(p) {};
        int blocksize;
        bool free;
        unsigned char *blockptr;	    
};


#endif
