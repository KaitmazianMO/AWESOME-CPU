
#ifndef COMMANDS_H 
#define COMMANDS_H

#ifndef NULL
#define NULL 0
#endif

#include <string.h>
#include <ctype.h>

typedef unsigned char cmd_t;

const cmd_t FLAG_OFF           = 0x3F;  // cmd & flag => real_cmd
const cmd_t REGISTER_FLAG      = 0x80;  // cmd | flag => register_cmd
const cmd_t MEMORY_ACCESS_FLAG = 0x40;  // cmd | flag => mem_cmd

enum CMD
    {
    CMD_UNKNOWN = 0,

    CMD_PUSH,
    CMD_POP,
    CMD_ADD,        
    CMD_SUB,        
    CMD_MULT,       
    CMD_DIV,
    CMD_NEG,        
    CMD_SQRT,       
    CMD_SIN,        
    CMD_COS,
    CMD_OUT,
    CMD_IN,
    CMD_DUMP,
    CMD_CALL,
    CMD_RET,
    CMD_JB,
    CMD_JBE,
    CMD_JA,
    CMD_JAE,
    CMD_JE,
    CMD_JNE,    
    CMD_JMP,
    CMD_HET,
    CMD_END,
    CMD_LABEL,
    };                 

struct Command
    {
    const char *string;
    cmd_t value;
    };

static Command PUSH  = {"push", CMD_PUSH},
               POP   = {"pop",  CMD_POP},
               ADD   = {"add",  CMD_ADD},
               SUB   = {"sub",  CMD_SUB},    
               MULT  = {"mult", CMD_MULT},
               DIV   = {"div",  CMD_DIV},
               NEG   = {"neg",  CMD_NEG},
               SQRT  = {"sqrt", CMD_SQRT},
               SIN   = {"sin",  CMD_SIN},
               COS   = {"cos",  CMD_COS},
               OUT   = {"out",  CMD_OUT},
               IN    = {"in",   CMD_IN},
               JMP   = {"jmp",  CMD_JMP},
               CALL  = {"call", CMD_CALL},
               RET   = {"ret",  CMD_RET},         
               JB    = {"jb",   CMD_JB},
               JBE   = {"jbe",  CMD_JBE},
               JA    = {"ja",   CMD_JA},
               JAE   = {"jae",  CMD_JAE},
               JE    = {"je",   CMD_JE},
               JNE   = {"jne",  CMD_JNE}, 
               DUMP  = {"dump", CMD_DUMP},
               HET   = {"het",  CMD_HET},
               END   = {"end",  CMD_END},
               LABEL = {NULL,   CMD_LABEL},
               UNKNOWN 
                    = {NULL,   CMD_UNKNOWN};
 
// Sorting pointers to elements, not the elements.
static Command *ASSEMBLER_COMMANDS[] = 
    {
    &PUSH,
    &POP,
    &ADD,
    &SUB,    
    &MULT,
    &DIV,
    &NEG,
    &SQRT,
    &SIN,
    &COS,
    &OUT,
    &IN,
    &JMP,
    &DUMP,
    &CALL,
    &RET,
    &JB,
    &JBE,
    &JA,
    &JAE,
    &JE,
    &JNE,
    &HET,
    &END, 
    &UNKNOWN,
    };
    
static int commandCompare (const void *cmd1, const void *cmd2)
    {
    Command *cmdl = *(Command **)cmd1;
    Command *cmdr = *(Command **)cmd2;


    if (cmdl->string == NULL) return 1;
    if (cmdr->string == NULL) return -1;

    return strcmp (cmdl->string, cmdr->string);
    }

#endif