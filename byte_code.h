#ifndef BYTE_CODE_H
#define BYTE_CODE_H

#include <stdlib.h>
#include <stdio.h>

#include "errors.h"

typedef unsigned char byte_t;


struct ByteCode
    { 
    byte_t *data;
    size_t  pos;     
    size_t  size;    
    };
 
    
void byteCodeCtor   (ByteCode *bcode, size_t size);
void byteCodeDtor   (ByteCode *bcode);
void byteCodeResize (ByteCode *bcode, size_t new_size);

#endif