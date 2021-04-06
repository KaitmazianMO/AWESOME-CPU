#ifndef LISTING_H
#define LISTING_H

#include <stdio.h>
#include <string>
#include <initializer_list>

__attribute__ ((unused))
static size_t N_LISTING_TABS = 0;

#define HEAD_INFO  { __FILE__, __func__}
#define ASSEMBLER_LISTING( ... )   printIndent (listing); fprintf (listing, "%s(): ", __func__);  fprintf (listing, __VA_ARGS__); \
                                                          fprintf (listing, " (%s)\n", __FILE__); fflush (listing);    

#define NEW_ASSEMBLER_LISTING_BLOCK( ... )  printIndent (listing);                                                           \
                                            Listing new_block (listing, HEAD_INFO);                                          \
                                            fprintf (listing, "%s (", __func__);                                             \
                                            fprintf (listing, __VA_ARGS__);                                                  \
                                            fprintf (listing, ") in file \"%s\"\n", __FILE__);                               \

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
const char *memoryDump (const void *mem, size_t size);

#endif  