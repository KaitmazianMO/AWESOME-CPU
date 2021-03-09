#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "assembler.h"
 
//g++ -Wall asm_main.cpp assembler.h assembler.cpp comands.h label.h label.cpp errors.h byte_code.h byte_code.cpp text.h text.cpp listing.h listing.cpp -o assembler
int main (int argc, char *argv[]) 
    {                   
    if (argc != 4)
        {
        printf ("Wrong input format. The format is <sourceFile> <binaryFile> <lisitngFile>\n");
        return INVALID_USER;
        }

    printf ("start assembling...\n"); 

    Assembler *asm_ptr = newAssembler (argv [3]);                         //           src       bin       lst
    NEW_ASSEMBLER_LISTING_BLOCK ("%d, \"%s\", \"%s\", \"%s\", \"%s\"", argc, argv [0], argv [1], argv [2], argv [3])

    int err = translateFile (asm_ptr, argv [1]);          

    if (!err)
        writeByteCode (asm_ptr, argv [2]);       

    ASSEMBLER_BLOCK_PREMATURE_COMPLETION()
    asm_ptr = dellAssembler (asm_ptr);
    printf ("finish assembling(%d)\n", err);    

    return err;
    }