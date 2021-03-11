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
static const char   NO_DELIM_FIELDS[]      = "[#]";

typedef double arg_t;

using namespace std;


/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  @todo writeCommand with flags@
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

typedef const char * cstring_t;

class Assembler      
{
public:
    Assembler (cstring_t src_code_file_name, cstring_t listing_file_name);
   ~Assembler();
    int translateCode();
    void writeByteCode (const char *file_name);
  
private:
    Text code;

    ByteCode byte_code;
    Label *label[NHASH];
    
    FILE *listing;

    void     writeArgument       (const void *arg, size_t arg_size);
    void     writeData           (const void *value, size_t value_size);
    void     writeCommand        (const Command *cmd, cmd_t cmd_type = 0x00);
    Command *identifyCommand     (const char* str);
    bool     enoughSpaseForValue (size_t value_size);
    Label   *addLabel            (const char *label_str);

    void trycatch_assemblerLabelCommandProcessing (Token **tok); 
    void          assemblerLabelCommandProcessing (Token *asm_label);
    Errors assemblerPushPopCommandsProcessing (Command *cmd, Token **tok);
    Errors assemblerJumpCommandProcessing  (Command *jmp_cmd, Token **tok);

    int translateMemoryAccesByRegister (Token *tok, unsigned char *arg_buf);
    int translateMemoryAccesByNumber (Token *tok, unsigned char *arg_buf);
    int translateRegisterArgument (Token *tok, unsigned char *arg_buf);
    int transateNumberArgument (Token *tok, unsigned char *arg_buf);
    int translateArgument (Token *tok, unsigned char *arg_buf);
    cmd_t identifyArgumentType (const Token *tok);
};


#define NELEMS( array ) (sizeof (array) / sizeof (array[0]))

#endif