#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <errno.h>
 
#include "comands.h" 
#include "buffer.h"
#include "text.h"
#include "errors.h" 
#include "byte_code.h"
#include "label.h"
#include "listing.h"


static const size_t DEFAUTL_BYTE_CODE_SIZE = 1024;              
static const size_t GROW_COEFFICIENT       = 2;
static const char   DELIM[]                = " \t\n\r\0";

typedef double arg_t;

using namespace std;


struct Assembler      
    {
    ByteCode byte_code;
    Label *label[NHASH];

    FILE *listing;
    };


void       assemblerCtor (Assembler *asm_ptr, const char *listing_file_name);
void       assemblerDtor (Assembler *asm_ptr);
Assembler *newAssembler  (const char *listing_file_name);
Assembler *dellAssembler (Assembler *asm_ptr); 
 
void     removeComments      (Buffer *buf);
void     translateFile       (Assembler *asm_ptr, const char *file_name);
void     translateCode       (Assembler *asm_ptr, Text *code);
void     writeArgument       (Assembler *asm_ptr, const void *arg, size_t arg_size);
char    *getArgument         (Buffer *buf);
byte_t   getRegisterNum      (const char *reg);
void     setCommandFlag      (Assembler *asm_ptr, byte_t flag);
bool     isCommand           (char *str);
void     writeCommand        (Assembler *asm_ptr, const Command *cmd);
Command *identifyCommand     (const char* str);
void     writeData           (Assembler *asm_ptr, const void *value, size_t value_size);
void     writeByteCode       (Assembler *asm_ptr, const char *file_name);
bool     enoughSpaseForValue (Assembler *asm_ptr, size_t value_size);
char    *strtokList          (Assembler *asm_ptr, char *buf, const char *delim);
void     addLabel            (Assembler *asm_ptr, const char *label);

#define NELEMS( array ) (sizeof (array) / sizeof (array[0]))


#endif