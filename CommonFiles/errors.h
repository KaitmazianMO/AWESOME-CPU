#ifndef ERRORS_H
#define ERRORS_H

// Critical errors. Stops program
#define CATCH( cond, error )  if (cond) { printf ("\n"                                                       \
                                                  "***************************************************\n"    \
                                                  "**\n"                                                     \
                                                  "**   Error    : %s (%d)\n"                                \
                                                  "**   File     : %s\n"                                     \
                                                  "**   Function : %s\n"                                     \
                                                  "**   Line     : %d\n"                                     \
                                                  "**   Condition: %s\n"                                     \
                                                  "**\n"                                                     \
                                                  "***************************************************\n",   \
                                                  #error, error, __FILE__,                                   \
                                                  __func__, __LINE__, #cond);                                \
                                          exit (error); }

#define VERIFY_ASSEMBLER    CATCH (!byte_code.data, NULL_BYTE_CODE_PTR);     \
                            CATCH (byte_code.pos >                           \
                                   byte_code.size,  POSITION_MORE_THAN_SIZE) \
                            CATCH (!listing,        NULL_LISTING_FILE_PTR);  \

#define VERIFY_BUFFER       CATCH (!buffer,       NULL_BUFFER_PTR);      \
                            CATCH (!buffer->data, NULL_BUFFER_DATA_PTR); \

#define VERIFY_BYTE_CODE    CATCH (!bcode,                   NULL_BYTE_CODE_PTR)       \
                            CATCH (bcode->pos > bcode->size, POSITION_MORE_THAN_SIZE)

#define VERIFY_CPU          CATCH (!cpu,            NULL_CPU_PTR)              \
                            CATCH (cpu->bcode.pos >                            \
                                   cpu->bcode.size, POSITION_MORE_THAN_SIZE)   \

#define TRANSLIATION_ERROR( ... )    { printf ("%s(%zu): ", code.getFileName(), code.getLineNumber (token)); printf (__VA_ARGS__); printf ("\n"); }

enum Errors
    {
    NOT_ERROR = 0,
/////////////////////
/// Common errors ///
/////////////////////
    NULL_PTR,
    NULL_FILE_NAME_PTR,
    NULL_FILE_PTR,

////////////////////////
/// Assembler errors ///
////////////////////////
    NULL_ASSEMBLER_PTR,
    NULL_BYTE_CODE_PTR,
    WRONG_ARGUMENT_FORMAT,
    POSITION_MORE_THAN_SIZE,
    NULL_LISTING_FILE_PTR,
    CANT_PROCESS_AN_ARGUMENT,
    UNCNOWN_COMMAND,

////////////////////////
/////  CPU errors  /////
////////////////////////    
    NULL_CPU_PTR,
    WRONG_REGISTER_NUMBER,

//////////////////////////////////
/////  �omputational errors  /////
//////////////////////////////////
    DIVISION_BY_ZERO,
    ROOT_OF_A_NEGATIVE_NUMBER,


/////////////////////
/// Buffer errors ///
/////////////////////
    NULL_BUFFER_PTR,
    NULL_BUFFER_DATA_PTR,
    NEGATIVE_BUFFER_SIZE,
    
///////////////////////
/// Function errors ///
///////////////////////
    REALLOCTAION_ERROR,
    NOT_ANOUGTH_ARGUNETS,
    UNKNOWN_LABEL,
    COPY_ERROR,
    NAN_ARGUMENT,
    FSTAT_RETURNED_NOT_NULL,

    NULL_CODE_PTR,
    UNKNOWN_COMMAND,
    WATAFAK,
    INVALID_USER,
    };
 
#endif