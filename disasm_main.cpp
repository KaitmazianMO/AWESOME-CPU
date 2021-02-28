#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "assembler.h"
#include "CPU.h"
#include "disassembler.h"

int main (int argc, char *argv[])
{
    disAssembly (argv[1], argv[2]);

    return 0;
}
 