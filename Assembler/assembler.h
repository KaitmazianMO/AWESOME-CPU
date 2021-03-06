#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <errno.h>
 
#include "comands.h" 
#include "../CommonFiles/text.h"
#include "../CommonFiles/errors.h" 
#include "byte_code.h"
#include "label.h"
#include "../CommonFiles/listing.h"


static const size_t DEFAUTL_BYTE_CODE_SIZE = 1024;              
static const size_t GROW_COEFFICIENT       = 2;
static const char   DELIM[]                = " \t\n\r\0";  
static const char   NO_DELIM_FIELDS[]      = "[#](#)";

typedef const char * cstring_t;
typedef double arg_t;

/*
    @TODO mem dump fix
*/

class Assembler : NonCopyable      
{
public:
    Assembler (cstring_t src_code_file_name, cstring_t listing_file_name);
   ~Assembler();
    void writeByteCode (const char *file_name) const;
    int translateCode();

private:
    Text code;

    ByteCode byte_code;
    Label *label[NHASH] = {};
    
    FILE *listing;

    int translateCodeToBinary();
    void     writeArgument       (const void *arg, size_t arg_size);
    void     writeData           (const void *value, size_t value_size);
    void     writeCommand        (const Command *cmd, cmd_t cmd_type = 0x00);
    Command *identifyCommand     (const char* str) const;
    Label   *addLabel            (const char *label_str);
    bool     enoughSpaseForValue (size_t value_size) const;

    void trycatch_assemblerLabelCommandProcessing (Token **tok); 
    void          assemblerLabelCommandProcessing (Token *asm_label);
    Errors errprocesed_assemblerPushPopCommandsProcessing (Command *cmd, Token **tok);
    Errors             assemblerPushPopCommandsProcessing (Command *cmd, Token **tok);
    Errors errprocesed_assemblerJumpCommandProcessing (Command *jmp_cmd, Token **tok, bool are_all_labels_procesed);
    Errors             assemblerJumpCommandProcessing (Command *jmp_cmd, Token **tok);
    void unknownTokenProcessing (Token **tok);

    int   translateMemoryAccess (Token *tok, unsigned char *arg_buf)      const;
    int   translateVideoMemoryAccess (Token *tok, unsigned char *arg_buf) const;
    int   translateRegisterArgument (Token *tok, unsigned char *arg_buf)  const;
    int   transateNumberArgument (Token *tok, unsigned char *arg_buf)     const;
    int   translateArgument (Token *tok, unsigned char *arg_buf)          const;
    cmd_t identifyArgumentType (const Token *tok)                         const;
};

char *processRegisterArgument (char *cur_position, cmd_t *reg_ptr);
char *processNumberArgument (char *cur_position, arg_t *arg_ptr);

#define NELEMS( array ) (sizeof (array) / sizeof (array[0]))

#endif