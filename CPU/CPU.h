#ifndef CPU_H
#define CPU_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <iostream>
#include <GL/glut.h>
                                                            
#include "../Assembler/byte_code.h"
#include "../CommonFiles/errors.h"
#include "../CommonFiles/text.h"
#include "../Assembler/comands.h"
#include "../Assembler/label.h"
#include "../Assembler/assembler.h"

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
static const size_t VIDEO_RAM_AXIS_X_SIZE   = 500;
static const size_t VIDEO_RAM_AXIS_Y_SIZE   = 500;
static const size_t RAM_SIZE                = 1024;

struct CPU 
{
    ByteCode bcode;

    stack_arg_t  stack;
    arg_t        registers[NREGISTERS];
    stack_size_t call_stack;

    char RAM [RAM_SIZE] = {};
    bool VideoRAM [2*VIDEO_RAM_AXIS_Y_SIZE] [2*VIDEO_RAM_AXIS_X_SIZE] = {};

    void pushCommandProcessing (cmd_t push);    // -OFSET_X < x < +OFSET_X
    void popCommandProcessing (cmd_t pop);
    void jumpCommandsProcessing (cmd_t jmp);
    void addCommandProcessing();
    void subCommandProcessing();
    void divCommandProcessing();
    void sqrtCommandProcessing();
    void retCommandProcessing();
    void callCommandProcessing();
    void inCommandProcessing();
    void drawCommandProcessing();
};
 

void CPUctor (CPU *cpu, const char *bcode_file_name);
void CPUdtor (CPU *cpu);

int CPURun (CPU *cpu);
void readByteCodeFromFile (ByteCode *bcode, const char *bcode_file_name);
cmd_t getCommand (ByteCode *bcode);
size_t getLabelPointer (ByteCode *bcode);
char getRegisterNum (ByteCode *bcode);
int getAddress (ByteCode *bcode);
arg_t getNumberArgument (ByteCode *bcode);
void CPUDump (const CPU *cpu);

#endif
