#ifndef BUFFER_H
#define BUFFER_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "errors.h"

typedef char buff_t;

struct Buffer
    {
    Buffer (const char *file_name);
    buff_t *data;
    size_t  size;
    };


Buffer *newBuffer (const char *file_name);
Buffer *dellBuffer (Buffer *buffer);

char *findWord (char *str);
#define wordLen( str )  ( strcspn (str, " \n\r\t\0") )
size_t fileSize (FILE *file);

#endif
 