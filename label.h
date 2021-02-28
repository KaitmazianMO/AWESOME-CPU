#ifndef LABEL_H
#define LABEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "comands.h"
#include "errors.h"
#include "buffer.h"

typedef cmd_t pos_t;

const size_t NHASH = 256;

struct Label
{
    char *name;
    pos_t pos;
    Label *next;
};


Label *newLabel (const char *name, pos_t pos);
Label *addLabel (Label **head, Label *new_label); 
Label *findName (Label *head, const char *name);   
Label *freeList (Label *head);
size_t hash (const char *str);
bool isLabel (const char *str);

#endif 