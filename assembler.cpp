#include "assembler.h"


Assembler *newAssembler (const char *listing_file_name)
    {
    CATCH (!listing_file_name, NULL_FILE_NAME_PTR)

    Assembler *asm_ptr = (Assembler *)calloc (1, sizeof (*asm_ptr));
    CATCH (!asm_ptr, NULL_ASSEMBLER_PTR)

    assemblerCtor (asm_ptr, listing_file_name);

    qsort (ASSEMBLER_COMMANDS, NELEMS (ASSEMBLER_COMMANDS), 
           sizeof (ASSEMBLER_COMMANDS[0]), commandCompare);

    return asm_ptr;
    }

void assemblerCtor (Assembler *asm_ptr, const char *listing_file_name)
    {                 
    CATCH (!asm_ptr,           NULL_ASSEMBLER_PTR)
    CATCH (!listing_file_name, NULL_FILE_NAME_PTR)

    byteCodeCtor (&asm_ptr->byte_code, DEFAUTL_BYTE_CODE_SIZE);

    asm_ptr->listing = fopen (listing_file_name, "wb");
    CATCH (!asm_ptr->listing, NULL_LISTING_FILE_PTR)
    }

void translateFile (Assembler *asm_ptr, const char *file_name)
    {
    NEW_ASSEMBLER_LISTING_BLOCK ("%p, %s", (const void *)asm_ptr, file_name)

    Buffer *code_buffer = newBuffer (file_name); 
    removeComments (code_buffer);

    setLabels (asm_ptr, code_buffer);
    translateBuffer (asm_ptr, code_buffer);           

    dellBuffer (code_buffer);      
    }

void translateBuffer (Assembler *asm_ptr, Buffer *buffer)
    {                                    
    VERIFY_ASSEMBLER
    VERIFY_BUFFER

    NEW_ASSEMBLER_LISTING_BLOCK ("%p, %p", (const void *)asm_ptr, (const void *)buffer)

    for (char *token = strtokList (asm_ptr, buffer->data, DELIM); token; token = strtokList (asm_ptr, NULL, DELIM)) 
        {
        Command *asm_com = identifyCommand (token);
 
        arg_t arg     = 0;
        char *end     = NULL;
        char  reg     = 0;
        char *arg_str = NULL;
        cmd_t cmd = (cmd_t)asm_com->value;
        switch (cmd)
            {
            case CMD_PUSH:
                writeCommand (asm_ptr, asm_com);
              
                arg_str = strtokList (asm_ptr, NULL, DELIM);

                arg = strtod (arg_str, &end);
                if (end != arg_str)     //process the number
                    writeArgument (asm_ptr, &arg, sizeof (arg));

                else
                    {
                    reg = getRegisterNum (arg_str);
                    if (reg >= 0) 
                        {
                        setCommandFlag (asm_ptr, REGISTER_FLAG);
                        writeArgument  (asm_ptr, &reg, sizeof (reg));
                        }
                    else
                        CATCH (token, CANT_PROCESS_AN_ARGUMENT)
                    }
                break;

            case CMD_POP:
                writeCommand (asm_ptr, asm_com);

                arg_str = strtokList (asm_ptr, NULL, DELIM);

                reg = getRegisterNum (arg_str);
                if (reg >= 0)
                    writeArgument (asm_ptr, &reg, sizeof (reg));
                else
                    CATCH (token, CANT_PROCESS_AN_ARGUMENT)
                 break;

            case CMD_JMP:
                {
                writeCommand (asm_ptr, asm_com);
                
                token = strtok (NULL, DELIM);
                arg_str = token;
                
                Label *find = findName (asm_ptr->label[hash (arg_str)], arg_str);
                if (!find) CATCH (1 ,UNKNOWN_LABEL)

                writeArgument (asm_ptr, &find->pos, sizeof find->pos);
                break;    
                }
            case CMD_ADD:
                writeCommand (asm_ptr, asm_com);
                break;    
                
            case CMD_SUB:
                writeCommand (asm_ptr, asm_com);
                break;
                
            case CMD_MULT:
                writeCommand (asm_ptr, asm_com);
                break;  
                
            case CMD_DIV:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_NEG:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_SQRT:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_SIN:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_COS:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_OUT:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_IN:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_DUMP:
                writeCommand (asm_ptr, asm_com);
                break; 

            case CMD_HET:
                writeCommand (asm_ptr, asm_com);
                break;

            case CMD_END:
                writeCommand (asm_ptr, asm_com);
                break;

            default: 
                CATCH (1, UNCNOWN_COMMAND)
            }
        }
    }


Command* identifyCommand (const char* str)
    {
    Command cmd    = {str, CMD_UNKNOWN};
    Command *tmp[] = { &cmd };

    Command **result = 
    (Command **)bsearch (tmp, ASSEMBLER_COMMANDS, NELEMS (ASSEMBLER_COMMANDS), 
                         sizeof (ASSEMBLER_COMMANDS[0]), commandCompare);
    
    CATCH (!result, UNKNOWN_COMMAND)
    return *result;
    }

void setLabels (Assembler *asm_ptr, Buffer *buf)
    {
    VERIFY_ASSEMBLER
 
    size_t pos = 0;
    char *find = buf->data;
    char *next = buf->data;

    while ((find = findWord (next)))  
        {
        next = find + wordLen (find);
        if (isLabel (find))
            {
            size_t len = wordLen (find);   
            find[len - 1] = ' ';                   
            addLabel (&asm_ptr->label[hash (find)], newLabel (find, pos));
            memset (find, ' ', len);
            }

        else if (isCommand (find))        
            pos += sizeof (cmd_t);   
             
        else           
           pos += sizeof (arg_t);          
        }   
    }

void writeData (Assembler *asm_ptr, const void *value, size_t value_size)
    {
    NEW_ASSEMBLER_LISTING_BLOCK ("%p, %p, %zu", (const void *)asm_ptr, (const void *)value, value_size)

    VERIFY_ASSEMBLER
    
    if (!enoughSpaseForValue (asm_ptr, value_size))
        {
        size_t new_size = asm_ptr->byte_code.size * GROW_COEFFICIENT;
        byteCodeResize (&asm_ptr->byte_code, new_size);
        }
    
    bool copy_error = !memcpy (asm_ptr->byte_code.data + asm_ptr->byte_code.pos, value, value_size);
    CATCH (copy_error, COPY_ERROR)
    asm_ptr->byte_code.pos += value_size;

    ASSEMBLER_LISTING ("data dump %s with size %zu", memoryDump (value, value_size).c_str(), value_size);
    }

void writeCommand (Assembler *asm_ptr, const Command *cmd)
    {
    VERIFY_ASSEMBLER
    CATCH (!cmd, NULL_PTR)

    const cmd_t command = cmd->value;

    ASSEMBLER_LISTING ("gets command %s (%d)", cmd->string, cmd->value);

    writeData (asm_ptr, &command, sizeof (command)); 
    }

void writeArgument (Assembler *asm_ptr, const void *arg, size_t arg_size)
    {                                                         
    VERIFY_ASSEMBLER
    
    ASSEMBLER_LISTING ("get argument %lg with size %zu", (arg_size > 1) ? *(double *)arg : (double)*(byte_t *)arg, arg_size)

    writeData (asm_ptr, arg, arg_size);
    }

byte_t getRegisterNum (const char *reg)
    {
    CATCH (!reg, NULL_PTR)

    if (reg[0] == 'r' && reg[2] == 'x')
        if (isalpha (reg[1]))
            return reg[1] - 'a';

    return - 1;
    }

void writeByteCode (Assembler *asm_ptr, const char *file_name)
    {
    NEW_ASSEMBLER_LISTING_BLOCK ("%p, %s", (const void *)asm_ptr, file_name)

    VERIFY_ASSEMBLER
    CATCH (!file_name, NULL_FILE_NAME_PTR);

    FILE *file = fopen (file_name, "wb");
    CATCH (!file, NULL_FILE_PTR)

    byte_t *bcode =  asm_ptr->byte_code.data;
    fwrite (bcode,  sizeof (bcode[0]), asm_ptr->byte_code.pos + 1, file);

    fclose (file);
    file = NULL;
    }

void setCommandFlag (Assembler *asm_ptr, byte_t flag)
    {
    ByteCode *bcode = &asm_ptr->byte_code;
    bcode->data [bcode->pos - 1] |= flag;
    }

char *strtokList (Assembler *asm_ptr, char *buf, const char *delim)
{
    char *token = strtok (buf, delim);
    
    ASSEMBLER_LISTING("finds %s", token);

    return token;
}

void removeComments (Buffer *buf)
    {
    auto start = buf->data;
    for (auto com = strchr (start, ';'); com; com = strchr (com, ';'))
        for (; *com != '\n' && *com != '\0'; ++com)
            *com = ' ';
    }

bool enoughSpaseForValue (Assembler *asm_ptr, size_t value_size)
    {
    return asm_ptr->byte_code.pos + value_size < asm_ptr->byte_code.size;
    }
                                         
void assemblerDtor (Assembler *asm_ptr)
    {
    VERIFY_ASSEMBLER

    byteCodeDtor (&asm_ptr->byte_code);

    fclose (asm_ptr->listing);
    asm_ptr->listing = NULL;
    }

Assembler *dellAssembler (Assembler *asm_ptr)
    {                                    
    VERIFY_ASSEMBLER
    
    for (size_t i = 0; i < NHASH; ++i)
        freeList (asm_ptr->label[i]);

    assemblerDtor (asm_ptr);
    free (asm_ptr);

    return (Assembler *)NULL;
    } 

bool isCommand (char *str)
{
    const char *word = findWord (str);
    const size_t len = wordLen (word);
    
    for (size_t i = 0; i < len; ++i)
        if (!isalpha (word[i]) && word[i] != '_')
            return false;

    return true;
}
