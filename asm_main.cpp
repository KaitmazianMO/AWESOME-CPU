#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "assembler.h"
 
//g++ -Wall asm_main.cpp assembler.h assembler.cpp comands.h label.h label.cpp buffer.h  buffer.cpp errors.h byte_code.h byte_code.cpp listing.h listing.cpp -o assembler
int main (int argc, char *argv[]) 
    {                     
    printf ("start assembling...\n"); 

    Assembler *asm_ptr = newAssembler (argv [3]);                         //           src       bin       lst
    NEW_ASSEMBLER_LISTING_BLOCK ("%d, \"%s\", \"%s\", \"%s\", \"%s\"", argc, argv [0], argv [1], argv [2], argv [3])

    translateFile (asm_ptr, argv [1]);          

    for (size_t i = 0; i < 200; ++i)
        printf("[%2d] \'%c\' (%d) \n", i , asm_ptr->byte_code.data[i], (int)asm_ptr->byte_code.data[i]);

    writeByteCode (asm_ptr, argv [2]);       

    ASSEMBLER_BLOCK_PREMATURE_COMPLETION()
    asm_ptr = dellAssembler (asm_ptr);
    printf ("finish assembling\n");    
   
    return 0;
    }
 

