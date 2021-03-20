
#include "CPU.h"


#define STACK_PUSH( value )       ( stack_push (&cpu->stack, value)      ) 
#define STACK_POP()               ( stack_pop  (&cpu->stack)             ) 
#define STACK_PEEK()              ( stack_peek (&cpu->stack)             ) 
#define CALL_STACK_PUSH( value )  ( stack_push (&cpu->call_stack, value) ) 
#define CALL_STACK_POP()          ( stack_pop  (&cpu->call_stack)        ) 
#define CALL_STACK_PEEK()         ( stack_peek (&cpu->call_stack)        ) 

void CPUctor (CPU *cpu, const char *bcode_file_name) 
{
    CATCH (!bcode_file_name, NULL_FILE_NAME_PTR)
    
    cpu->bcode = {};
    byteCodeCtor         (&cpu->bcode, DEAFAULT_BYTE_CODE_SIZE + 1);
    readByteCodeFromFile (&cpu->bcode, bcode_file_name);

    cpu->stack = {};
    stack_ctor (&cpu->stack, CPU_STACK_INITIAL_SIZE);
    stack_ctor (&cpu->call_stack, CPU_STACK_INITIAL_SIZE);
}

void readByteCodeFromFile (ByteCode *bcode, const char *bcode_file_name) 
{
    FILE *bcode_file = fopen (bcode_file_name, "rb");
    CATCH (!bcode_file, NULL_FILE_PTR);     
    
    size_t bcode_size = fileSize (bcode_file);
    if (bcode->size < bcode_size) 
        byteCodeResize (bcode, bcode_size);
    
    bcode->size = fread (bcode->data, sizeof (byte_t), bcode_size, bcode_file);  

    fclose (bcode_file);
}

int CPURun (CPU *cpu) 
{
    VERIFY_CPU

    ByteCode *bcode = &cpu->bcode;
    arg_t *registers = cpu->registers;
    char * RAM = cpu->RAM;

    while (bcode->pos < bcode->size) 
    {
        cmd_t cmd = getCommand (bcode);

        char   regn = 0;
        int    addr = 0;
        arg_t  arg  = 0;
        arg_t  larg = 0;
        arg_t  rarg = 0;
        size_t ip   = 0;
        size_t absolute_address = 0;
        switch (cmd & FLAG_OFF) 
        {                                 
            case CMD_PUSH:
                cpu->pushCommandProcessing (cmd);
                break; 
                
            case CMD_POP:
                cpu->popCommandProcessing (cmd);
                break; 

            case CMD_ADD:
                STACK_PUSH (STACK_POP() + STACK_POP());
                break;

            case CMD_SUB:
                cpu->subCommandProcessing();
                break;

            case CMD_MULT:       
                STACK_PUSH (STACK_POP() * STACK_POP());
                break;

            case CMD_DIV:
                cpu->divCommandProcessing();
                break;

            case CMD_NEG:
                STACK_PUSH (-STACK_POP());
                break;
                
            case CMD_SQRT:
                cpu->sqrtCommandProcessing();
                break;

            case CMD_JMP:
            case CMD_JB:
            case CMD_JBE:
            case CMD_JA:
            case CMD_JAE:
            case CMD_JE:
            case CMD_JNE:
                cpu->jumpCommandsProcessing (cmd);
                break;

            case CMD_SIN: 
                STACK_PUSH (sin (STACK_POP()));
                break;

            case CMD_COS:
                STACK_PUSH (cos (STACK_POP()));
                break;

            case CMD_OUT:
                printf ("%lg\n", STACK_PEEK());
                break;

            case CMD_RET: 
                cpu->retCommandProcessing();
                break;

            case CMD_CALL: 
                cpu->callCommandProcessing();
                break;

            case CMD_IN:
                cpu->inCommandProcessing();
                break;

            case CMD_DUMP:
                CPUDump (cpu);
                break;

            case CMD_HET:
                CPUdtor (cpu);
                return 0;
                break;

            case CMD_END:
                return 0;
                break;

            default:
                printf ("wtf is this %d at %zu!?\n", cmd, bcode->pos);
                return UNCNOWN_COMMAND;
        }       
    }

    return 0;
}

cmd_t getCommand (ByteCode *bcode) 
{
    VERIFY_BYTE_CODE

    cmd_t cmd = (cmd_t)bcode->data[bcode->pos];
    bcode->pos += sizeof (cmd_t);
    return cmd;
}

arg_t getNumberArgument (ByteCode *bcode) 
{
    VERIFY_BYTE_CODE

    arg_t arg = *(arg_t *)(bcode->data + bcode->pos);
    bcode->pos += sizeof (arg_t);

    return arg;
}

int getAddress (ByteCode *bcode)
{
    int addr = *(int *)(bcode->data + bcode->pos);
    bcode->pos += sizeof (addr);
    return addr;
}

char getRegisterNum (ByteCode *bcode) 
{
    VERIFY_BYTE_CODE

    char regn = bcode->data[bcode->pos];
    bcode->pos += sizeof (regn);

    if (regn >= 0 && regn < NREGISTERS)
        return regn;

    return -1;
}

size_t getLabelPointer (ByteCode *bcode)
{   
    return *(size_t *)(bcode->data + bcode->pos);
}

void CPUdtor (CPU *cpu) 
{
    VERIFY_CPU 

    byteCodeDtor (&cpu->bcode);
    stack_dtor   (&cpu->call_stack);
    stack_dtor   (&cpu->stack);
}