#ifndef CPU_H
#define CPU_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <float.h>
#include <math.h>
                                                            
#include "byte_code.h"
#include "errors.h"
#include "text.h"
#include "comands.h"
#include "label.h"
#include "assembler.h"

#define stack_t arg_t
#define NO_PROTECTION
#define NO_LOG
#include "stack.h"

#define stack_t size_t
#define NO_PROTECTION
#define NO_LOG
#include "stack.h"

static const size_t CPU_STACK_INITIAL_SIZE  = 8;
static const size_t DEAFAULT_BYTE_CODE_SIZE = 1024;
static const size_t NREGISTERS              = 8;

struct CPU 
{
    ByteCode bcode;

    stack_arg_t  stack;
    arg_t        registers[NREGISTERS];
    stack_size_t call_stack;
};
 

void CPUctor (CPU *cpu, const char *bcode_file_name);
void CPUdtor (CPU *cpu);

int CPURun (CPU *cpu);
void readByteCodeFromFile (ByteCode *bcode, const char *bcode_file_name);
cmd_t getCommand (ByteCode *bcode);
size_t getLabelPointer (ByteCode *bcode);
char getRegisterNum (ByteCode *bcode);
arg_t getArgument (ByteCode *bcode);
void CPUDump (const CPU *cpu);

#endif
