#ifndef LISTING_H
#define LISTING_H

#include <stdio.h>
#include <string>
#include <initializer_list>

static size_t N_LISTING_TABS = 0;

#define HEAD_INFO  { __FILE__, __func__}
#define ASSEMBLER_LISTING( ... )   printIndent (asm_ptr->listing); fprintf (asm_ptr->listing, "%s(): ", __func__);  fprintf (asm_ptr->listing, __VA_ARGS__); \
                                                                   fprintf (asm_ptr->listing, " (%s)\n", __FILE__); fflush (asm_ptr->listing);    

#define NEW_ASSEMBLER_LISTING_BLOCK( ... )  printIndent (asm_ptr->listing);                                                           \
                                            Listing new_block (asm_ptr->listing, HEAD_INFO);                                          \
                                            fprintf (asm_ptr->listing, "%s (", __func__);                                             \
                                            fprintf (asm_ptr->listing, __VA_ARGS__);                                                  \
                                            fprintf (asm_ptr->listing, ") in file \"%s\"\n", __FILE__);                               \

#define TEXT_LISTING( ... ) if (log) { printIndent (log); fprintf (log, "%s(): ", __func__); \
                                       fprintf (log, __VA_ARGS__);                           \
                                       fprintf (log, " (%s)\n", __FILE__); fflush (log); }   \

#define ASSEMBLER_BLOCK_PREMATURE_COMPLETION() new_block.~Listing();

struct INFO 
{
    const char *file;
    const char *func;    
};

class Listing 
{
public:
    Listing (FILE *listing, INFO info);

   ~Listing ();

private:
    FILE *listing_;
    INFO info_;
};

void printIndent (FILE *file);
std :: string memoryDump (const void *mem, size_t size);

#endif  