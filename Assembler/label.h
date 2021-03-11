#ifndef LABEL_H
#define LABEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "comands.h"
#include "../CommonFiles/errors.h"
#include "../CommonFiles/text.h"

const size_t NHASH = 256;

struct Label
{
    char *name;
    size_t pos;
    Label *next;
};


Label *newLabel (const char *name, size_t pos);
Label *pushBackLabel (Label **head, Label *new_label); 
Label *findName (Label *head, const char *name);   
Label *freeList (Label *head);
size_t strHash (const char *str);
void freeLabel (Label *label);
bool isLabel (const char *str);

#endif 