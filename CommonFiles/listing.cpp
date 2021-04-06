 #include "listing.h"
 
 
Listing :: Listing (FILE *listing, INFO info) : listing_ (listing), info_ (info)
{
    ++N_LISTING_TABS; 
}

Listing :: ~Listing() 
{ 
    if (listing_)
    {
        --N_LISTING_TABS;
        printIndent (listing_);
        fprintf (listing_, "%s: out from %s()\n", info_.file, info_.func); 
        fflush (listing_);
    }
    listing_ = NULL;
}

void printIndent (FILE *file)
{
    if (file)
        for (size_t i = 0; i < N_LISTING_TABS; ++i)
            fputc ('\t', file);
}

const char *memoryDump (const void *mem, size_t size)
{
    const size_t        n = 128;
    static char memhex[n] = {};

    if (mem)
        for (size_t i = 0; i < size && i < n; i += 2)
            sprintf (memhex + i, "%2X", *((const char *)mem + i));
    
    memhex [size] = '\0';
    return memhex;
}
