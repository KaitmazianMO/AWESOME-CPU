
#include "CPU.h"


CPU :: CPU (const char *bcode_file_name) 
{
    assert (bcode_file_name);
    assert (this);
    
    byteCodeCtor         (&bcode, DEAFAULT_BYTE_CODE_SIZE + 1);
    readByteCodeFromFile (&bcode, bcode_file_name);

    stack_ctor (&stack,      CPU_STACK_INITIAL_SIZE);
    stack_ctor (&call_stack, CPU_STACK_INITIAL_SIZE);
}

CPU :: ~CPU () 
{
    assert (this); 

    byteCodeDtor (&bcode);
    stack_dtor   (&call_stack);
    stack_dtor   (&stack);
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
    try
    {
        while (bcode->pos < bcode->size) 
        {
            cmd_t cmd = getCommand (bcode);

            switch (cmd & FLAG_OFF) 
            {                                 
                case CMD_PUSH:
                    cpu->pushCommandProcessing (cmd);
                    break; 

                case CMD_POP:
                    cpu->popCommandProcessing (cmd);
                    break; 

                case CMD_ADD:
                   cpu->addCommandProcessing();
                    break;

                case CMD_SUB:
                    cpu->subCommandProcessing();
                    break;

                case CMD_MULT:       
                    cpu->multCommandProcessing();
                    break;

                case CMD_DIV:
                    cpu->divCommandProcessing();
                    break;

                case CMD_NEG:
                    cpu->negCommandProcessing();
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
                    cpu->sinCommandProcessing();
                    break;

                case CMD_COS:
                    cpu->cosCommandProcessing();
                    break;

                case CMD_OUT:
                    cpu->outCommandProcessing();
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

                case CMD_DRAW:
                    cpu->drawCommandProcessing();

                case CMD_END:
                    return 0;

                default:
                    printf ("wtf is this %d at %zu!?\n", cmd, bcode->pos);
                    return UNCNOWN_COMMAND;
            }

        if (stk_err)
            throw std :: runtime_error ("Stack was disapoited in you!");       
        }
    }
    catch (std :: exception &ex)
    {
        std :: cout << ex.what() << '\n';
        return INVALID_USER;
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
