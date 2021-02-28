
#include "CPU.h"

#define STACK_PUSH( value )  ( stack_push (&cpu->stack, value) ) 
#define STACK_POP()          ( stack_pop  (&cpu->stack)        ) 
#define STACK_PEEK()         ( stack_peek (&cpu->stack)        ) 

void CPUctor (CPU *cpu, const char *bcode_file_name) 
{
    CATCH (!bcode_file_name, NULL_FILE_NAME_PTR)
    
    cpu->bcode = {};
    byteCodeCtor         (&cpu->bcode, DEAFAULT_BYTE_CODE_SIZE + 1);
    readByteCodeFromFile (&cpu->bcode, bcode_file_name);

    cpu->stack = {};
    stack_ctor (&cpu->stack, CPU_STACK_INITIAL_SIZE);
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

    while (bcode->pos < bcode->size) 
    {
        cmd_t cmd = getCommand (bcode);

        char  regn = 0;
        arg_t arg  = 0;
        arg_t larg = 0;
        arg_t rarg = 0;
        switch (cmd & FLAG_OFF) 
        {                                 
            case CMD_PUSH:
                if (cmd & REGISTER_FLAG) 
                {
                    regn = getRegisterNum (bcode);
                    if (regn < 0)
                        return WRONG_REGISTER_NUMBER;
                    STACK_PUSH (registers[regn]);
                }
                else 
                {
                    arg = getArgument (bcode);
                    STACK_PUSH (arg);
                }
                break; 
                
            case CMD_POP:
                regn = getRegisterNum (bcode);
                if (regn < 0)
                    return WRONG_REGISTER_NUMBER;
                registers[regn] = STACK_POP();
                break;

            case CMD_JMP:
                bcode->pos = getLabelPointer (bcode);
                break;

            case CMD_ADD:
                STACK_PUSH (STACK_POP() + STACK_POP());
                break;

            case CMD_SUB:
                rarg = STACK_POP();
                larg = STACK_POP();
                STACK_PUSH (larg - rarg);
                break;

            case CMD_MULT:       
                STACK_PUSH (STACK_POP() * STACK_POP());
                break;

            case CMD_DIV:
                rarg = STACK_POP();
                larg = STACK_POP();
                if (fabs (rarg) < DBL_EPSILON) 
                    return DIVISION_BY_ZERO;
                STACK_PUSH (larg / rarg);
                break;

            case CMD_NEG:
                STACK_PUSH (-STACK_POP());
                break;
                
            case CMD_SQRT:
                arg = STACK_POP();
                if (arg < 0) 
                    return ROOT_OF_A_NEGATIVE_NUMBER;
                STACK_PUSH (sqrt (arg));
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

            case CMD_IN:
                if (scanf ("%lg", &arg) != 1)
                    return CANT_PROCESS_AN_ARGUMENT;
                STACK_PUSH (arg);
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

arg_t getArgument (ByteCode *bcode) 
{
    VERIFY_BYTE_CODE

    arg_t arg = *(arg_t *)(bcode->data + bcode->pos);
    bcode->pos += sizeof (arg_t);

    return arg;
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

pos_t getLabelPointer (ByteCode *bcode)
{
    return *(pos_t *)(bcode->data + bcode->pos);
}

void CPUdtor (CPU *cpu) 
{
    VERIFY_CPU 

    byteCodeDtor (&cpu->bcode);
    stack_dtor   (&cpu->stack);
}

void CPUDump (const CPU *cpu)
{
    for (size_t i = 0; i < NREGISTERS; ++i)
        printf ("r%cx:    %lg\n", 'a' + i, cpu->registers[i]);

    for (int i = 0; i < cpu->stack.capacity; ++i)
        printf ("stk[%d]: %lg\n", i,       cpu->stack.data[i]);
}