
#include "byte_code.h"


void byteCodeCtor (ByteCode* bcode, size_t size)
    {
    CATCH (!bcode, NULL_BYTE_CODE_PTR)
    bcode->data = (byte_t *)calloc (size, sizeof (*bcode->data));
    CATCH (!bcode->data, NULL_BYTE_CODE_PTR)
    
    bcode->pos  = 0;
    bcode->size = size;

    VERIFY_BYTE_CODE
    }
 
void byteCodeResize (ByteCode *bcode, size_t new_size)
    {
    VERIFY_BYTE_CODE

    byte_t *bcode_start = bcode->data;

    byte_t *new_bcode = (byte_t *)realloc (bcode_start, new_size * sizeof (byte_t));
    CATCH (!new_bcode, REALLOCTAION_ERROR)

    bcode->size = new_size;
    bcode->data = new_bcode;
    }

void byteCodeDtor (ByteCode* bcode)
    {
    VERIFY_BYTE_CODE
    free (bcode->data);
    }