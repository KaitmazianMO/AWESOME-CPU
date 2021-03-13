#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "assembler.h"
 
//g++ -Wall asm_main.cpp assembler.h assembler.cpp comands.h label.h label.cpp  ../CommonFiles/errors.h byte_code.h byte_code.cpp ../CommonFiles/text.h ../CommonFiles/text.cpp ../CommonFiles/listing.h ../CommonFiles/listing.cpp -o assembler
int main (int argc, char *argv[]) 
    {                   
    if (argc != 4)
        {
        printf ("Wrong input format. The format is <sourceFile> <binaryFile> <lisitngFile>\n");
        return INVALID_USER;
        }

    printf ("start assembling...\n"); 

    Assembler assembler (argv [1], argv [3]);
   
    int err = assembler.translateCode();

    if (!err)
        assembler.writeByteCode (argv [2]);       

    printf ("finish assembling(%d)\n", err);    

    return err;
    }