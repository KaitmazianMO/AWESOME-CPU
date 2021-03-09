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

std :: string memoryDump (const void *mem, size_t size)
{
    const size_t n = 50;
    char memhex[2*n] = {};

    if (mem)
        for (size_t i = 0; i < size && i < 2*n; ++i)
            sprintf (memhex + 2*i, "%02X", (int)*((const char *)mem + i));

    return std :: string (memhex);
}
