#include "assembler.h"

static bool err = false;

Assembler :: Assembler (cstring_t src_code_file_name, cstring_t listing_file_name)
: code (src_code_file_name)
{
    assert (src_code_file_name);
    assert (listing_file_name);

    byteCodeCtor (&byte_code, DEFAUTL_BYTE_CODE_SIZE);

    listing = fopen (listing_file_name, "wb");
    assert (listing_file_name);

    code.setLogFile (listing);

    qsort (ASSEMBLER_COMMANDS, NELEMS (ASSEMBLER_COMMANDS), 
           sizeof (ASSEMBLER_COMMANDS[0]), commandCompare);
}

Assembler :: ~Assembler()
{
    VERIFY_ASSEMBLER

    byteCodeDtor (&byte_code);
    fclose (listing);
    listing = nullptr;
}

int Assembler :: translateCode()
{
    VERIFY_ASSEMBLER

    code.fillStringsAfter (';', ' ');
    code.tokenizeText (DELIM, NO_DELIM_FIELDS, NULL_TERMINATED);

    err = translateCodeToBinary();
    if (!err)
        err = translateCodeToBinary();         

    return err;  
}

int Assembler :: translateCodeToBinary()
{                              
    VERIFY_ASSEMBLER

    byte_code.pos = 0;
    static bool are_all_labels_procesed = false;  
    for (Token *token = code.getNextToken (NULL); token; token = code.getNextToken (token)) 
    {
        Command *asm_com = identifyCommand (token->str);
        cmd_t cmd = (cmd_t)asm_com->value;
        switch (cmd)
        {
            case CMD_PUSH:
            case CMD_POP:
                errprocesed_assemblerPushPopCommandsProcessing (asm_com, &token);
                break;

            case CMD_JB:
            case CMD_JBE:
            case CMD_JA:
            case CMD_JAE:
            case CMD_JE:
            case CMD_JNE:
            case CMD_JMP:
            case CMD_CALL:
                errprocesed_assemblerJumpCommandProcessing (asm_com,  &token, are_all_labels_procesed);
                break;    

            case CMD_LABEL:
                trycatch_assemblerLabelCommandProcessing (&token);
                break;   

            case CMD_RET:
            case CMD_ADD:
            case CMD_SUB:
            case CMD_MULT:
            case CMD_DIV:
            case CMD_NEG:
            case CMD_SQRT:
            case CMD_SIN:
            case CMD_COS:
            case CMD_OUT:
            case CMD_IN:
            case CMD_DUMP:
            case CMD_HET:
            case CMD_END:
                writeCommand (asm_com);
                break;

            default: 
                unknownTokenProcessing (&token);

        }
    }

    are_all_labels_procesed = true;

    return err;
}

//!{===================================
//!     PUSH POP COMMANDS FUNCTIONS
//!}===================================

Errors Assembler :: errprocesed_assemblerPushPopCommandsProcessing (Command *cmd, Token **tok)
{
    int pp_err = assemblerPushPopCommandsProcessing (cmd, tok);
    if (pp_err)
    {
        Token *token = *tok;
        if (pp_err == CANT_PROCESS_AN_ARGUMENT)
            TRANSLIATION_ERROR ("cant't process an argument %s", token->str)

        else if (pp_err == WRONG_ARGUMENT_FORMAT)
            TRANSLIATION_ERROR ("wrong argument format: %s with %s", token->str, cmd->string);
        
        token = code.getLastLineToken (token);  
        err = true;                   
    }
}

Errors Assembler :: assemblerPushPopCommandsProcessing (Command *cmd, Token **tok)
{
    VERIFY_ASSEMBLER
    assert (cmd);
    assert (tok);

    Token *cmd_token = *tok;
    Token *arg_token = code.getNextToken (cmd_token);
    *tok = arg_token;

    cmd_t cmd_type = identifyArgumentType (arg_token);

    cmd_t arg_buf [2*sizeof (arg_t)] = {};
    int arg_size = translateArgument (arg_token, arg_buf);
    if (arg_size < 1)
        return CANT_PROCESS_AN_ARGUMENT;

    if (cmd->value == CMD_POP && cmd_type == 0) // POP without any flags => ??? pop 3 ???
        return WRONG_ARGUMENT_FORMAT;

    writeCommand (cmd, cmd_type);
    writeArgument (arg_buf, arg_size);
}

int Assembler :: translateArgument (Token *tok, unsigned char *arg_buf)
{
    assert (tok);
    assert (arg_buf);

    int arg_size   = -1;
    cmd_t arg_type = identifyArgumentType (tok);
    
    if (arg_type & MEMORY_ACCESS_FLAG)
        {
        if (arg_type & REGISTER_FLAG)
            arg_size = translateMemoryAccesByRegister (tok, arg_buf);
        else
            arg_size = translateMemoryAccesByNumber (tok, arg_buf);
        }
    else
        if (arg_type & REGISTER_FLAG)
            arg_size = translateRegisterArgument (tok, arg_buf);
        else
            arg_size = transateNumberArgument (tok, arg_buf);

    return arg_size;
}

int Assembler :: translateMemoryAccesByRegister (Token *tok, unsigned char *arg_buf)
{
    assert (tok);
    assert (arg_buf);

    cmd_t reg = 0;
    int   num = 0;
    char *cur_possition = tok->str;
    char *last_possition = tok->str + tok->size - 1;

    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != '[') return -1;
    
    //find register format
    ++cur_possition;
    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != 'r') return -1;

    ++cur_possition;
    if (!islower (*cur_possition)) return -1;
      
    reg = *cur_possition - 'a';
    memcpy (arg_buf, &reg, sizeof (reg));

    ++cur_possition;
    if (*cur_possition != 'x') return -1;
      
    ++cur_possition;
    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != ',') return -1;

    // find number
    ++cur_possition;
    while (isspace (*cur_possition)) ++cur_possition;
    if (!isdigit (*cur_possition) && *cur_possition != '+' && *cur_possition != '-') return -1;

    char *num_end = NULL; 
    num = strtol (cur_possition, &num_end, 10);
    if (cur_possition == num_end) return -1;
    memcpy (arg_buf + sizeof (reg), &num, sizeof (num));

    cur_possition = num_end;

    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != ']') return -1;

    if (cur_possition != last_possition) return -1;

    return sizeof (reg) + sizeof (num);
}

int Assembler :: transateNumberArgument (Token *tok, unsigned char *arg_buf)
{
    assert (tok);
    assert (arg_buf);

    arg_t num = 0;
    char *cur_possition = tok->str;
    char *last_possition = tok->str + tok->size;

    while (isspace (*cur_possition)) ++cur_possition;
    if (!isdigit (*cur_possition) && *cur_possition != '+' && *cur_possition != '-') return -1;

    char *num_end = NULL; 
    num = strtod (cur_possition, &num_end);
    if (cur_possition == num_end) return -1;
    memcpy (arg_buf, &num, sizeof (num));

    cur_possition = num_end;       

    cur_possition;
    while (cur_possition < last_possition) 
        if (!isspace (*cur_possition++))
            return -1;

    return sizeof (num);     
}

int Assembler :: translateRegisterArgument (Token *tok, unsigned char *arg_buf)
{
    assert (tok);
    assert (arg_buf);

    cmd_t reg = 0;
    char *cur_possition = tok->str;
    char *last_possition = tok->str + tok->size;

    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != 'r') return -1;

    ++cur_possition;
    if (!islower (*cur_possition)) return -1;
        
    reg = *cur_possition - 'a';
    memcpy (arg_buf, &reg, sizeof (reg));

    ++cur_possition;
    if (*cur_possition != 'x') return -1;    

    ++cur_possition;
    while (cur_possition < last_possition) 
        if (!isspace (*cur_possition++))
            return -1;

    return sizeof (reg);
}

int Assembler :: translateMemoryAccesByNumber (Token *tok, unsigned char *arg_buf)
{
    assert (tok);
    assert (arg_buf);

    int num = 0;
    char *cur_possition = tok->str;

    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != '[') return -1;    

    ++cur_possition;
    while (isspace (*cur_possition)) ++cur_possition;
    if (!isdigit (*cur_possition) && *cur_possition != '+' && *cur_possition != '-') return -1;

    char *num_end = NULL; 
    num = strtol (cur_possition, &num_end, 10);
    if (cur_possition == num_end) return -1;
    memcpy (arg_buf, &num, sizeof (num));

    cur_possition = num_end;

    while (isspace (*cur_possition)) ++cur_possition;
    if (*cur_possition != ']') return -1;

    return sizeof (num);
}
    
cmd_t Assembler :: identifyArgumentType (const Token *tok)
{
    cmd_t type = 0u;

    if (strchr (tok->str, '['))
        type |= MEMORY_ACCESS_FLAG;

    if (strchr (tok->str, 'r'))
        type |= REGISTER_FLAG;

    return type;
}

//!{===================================
//!         LABEL FUNCTIONS
//!}===================================

void Assembler :: trycatch_assemblerLabelCommandProcessing (Token **tok) 
{
    VERIFY_ASSEMBLER
    assert (tok);

    Token *token = *tok;
    try
    {
        assemblerLabelCommandProcessing (*tok);
    }
    catch (exception &ex)  
    {
            TRANSLIATION_ERROR ("same label names(%s) for differen pointers", token->str);
            *tok = code.getLastLineToken (token);
            err = true;
    }    
}

void Assembler :: assemblerLabelCommandProcessing (Token *asm_label)
{
    VERIFY_ASSEMBLER
    assert (asm_label);

    auto tmp = asm_label->str [asm_label->size - 1];
    asm_label->str [asm_label->size - 1] = '\0';

    Label *res = addLabel (asm_label->str);
    asm_label->str [asm_label->size - 1] = tmp;  

    if (!res)
        throw exception();
}

//!{===================================
//!      JUMP COMMANDS FUNCTIONS
//!}===================================

Errors Assembler :: errprocesed_assemblerJumpCommandProcessing (Command *jmp_cmd, Token **tok, bool are_all_labels_procesed)
{
    int jmp_err = assemblerJumpCommandProcessing  (jmp_cmd,  tok);
    if (jmp_err && are_all_labels_procesed)
    {
        Token *token = *tok;
        TRANSLIATION_ERROR ("invalid label %s", token->str);
        token = code.getLastLineToken (token);
        err = true;
    }
}

Errors Assembler :: assemblerJumpCommandProcessing  (Command *jmp_cmd, Token **tok)
{
    VERIFY_ASSEMBLER   
    assert (jmp_cmd);
    assert (tok);

    writeCommand (jmp_cmd);
    *tok = code.getNextToken (*tok);

    Label *find = findName (label [strHash ((*tok)->str)], (*tok)->str);
    if (!find)
    {
        byte_code.pos += sizeof (find->pos);
        return UNKNOWN_LABEL;
    }

    writeArgument (&find->pos, sizeof (find->pos));   

    return NOT_ERROR; 
}

//!{=======================================
//!     NOT COMMAND ORIENTED FUNCTIONS
//!}=======================================

void Assembler :: unknownTokenProcessing (Token **tok)
{
    Token *token = *tok;
    TRANSLIATION_ERROR ("unknown token \"%s\"", token->str);
    *tok = code.getLastLineToken (token);
    err = true;
}

Command* Assembler :: identifyCommand (const char* str)
{
    assert (str);

    if (isLabel (str)) return &LABEL;

    Command cmd    = {str, CMD_UNKNOWN};
    Command *tmp[] = { &cmd };

    Command **result = 
    (Command **)bsearch (tmp, ASSEMBLER_COMMANDS, NELEMS (ASSEMBLER_COMMANDS), 
                         sizeof (ASSEMBLER_COMMANDS[0]), commandCompare);
    
    if (!result)
        return &UNKNOWN;
    
    return *result;
}

void Assembler :: writeData (const void *value, size_t value_size)
{
    VERIFY_ASSEMBLER
    assert (value);

    if (!enoughSpaseForValue (value_size))
        {
        size_t new_size = byte_code.size * GROW_COEFFICIENT;
        byteCodeResize (&byte_code, new_size);
        }
    
    bool copy_error = !memcpy (byte_code.data + byte_code.pos, value, value_size);
    CATCH (copy_error, COPY_ERROR)
    byte_code.pos += value_size;

    ASSEMBLER_LISTING ("data dump %s with size %zu", memoryDump (value, value_size).c_str(), value_size);
}

void Assembler :: writeCommand (const Command *cmd, cmd_t cmd_type)
{
    VERIFY_ASSEMBLER
    assert (cmd);

    const cmd_t command = cmd->value | cmd_type;

    ASSEMBLER_LISTING ("gets command %s (%d)", cmd->string, cmd->value);

    writeData (&command, sizeof (command)); 
}

void Assembler ::writeArgument (const void *arg, size_t arg_size)
{                                                         
    VERIFY_ASSEMBLER
    CATCH (!arg, NULL_PTR)
    
    ASSEMBLER_LISTING ("get argument %lg with size %zu", (arg_size > 1) ? *(double *)arg : (double)*(byte_t *)arg, arg_size)

    writeData (arg, arg_size);
}

Label *Assembler :: addLabel (const char *label_str)
{
    VERIFY_ASSEMBLER
    CATCH (!label, NULL_PTR)
    return pushBackLabel (&label [strHash (label_str)], newLabel (label_str, byte_code.pos));
}

void Assembler :: writeByteCode (const char *file_name)
{
    VERIFY_ASSEMBLER
    assert (file_name);

    FILE *file = fopen (file_name, "wb");
    CATCH (!file, NULL_FILE_PTR)

    byte_t *bcode =  byte_code.data;
    fwrite (bcode,  sizeof (bcode[0]), byte_code.pos + 1, file);

    fclose (file);
    file = NULL;
}

bool Assembler :: enoughSpaseForValue (size_t value_size)
{
    VERIFY_ASSEMBLER

    return byte_code.pos + value_size < byte_code.size;
}
