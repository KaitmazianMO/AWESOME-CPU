#include "label.h"

Label *newLabel (const char *name, size_t pos)
{
    Label *label = (Label *)calloc (1, sizeof (*label));
    if (!label) return NULL;

    label->name = (char *)calloc (wordLen (name) + 1, sizeof (*name));
    memcpy (label->name, name, wordLen (name));
    label->pos = pos;
    
    return label;
}

Label *addLabel (Label **head, Label *new_label)
{
    if (!*head) return *head = new_label;

    Label *tail = *head;

    while (tail->next)  
    {
        if (strcmp (tail->name, new_label->name) == 0) 
            if (tail->pos == new_label->pos)
                return new_label;
            else
            {
                free (new_label);
                return NULL;
            }
        tail = tail->next; 
    }    
    tail->next = new_label;
 
    return new_label;
}

Label *findName (Label *head, const char *name)
{
    assert (name);
    for (Label *find = head; find; find = find->next)
        if (find->name && strcmp (find->name, name) == 0)
            return find;
    
    return NULL;
}

Label *freeList (Label *head)
{
    Label *prev = head;
    Label *cur  = head;

    while (cur)
    {
        prev = cur; 
        cur = cur->next;
        free (prev);
    }
    free (cur);

    return NULL;
}

size_t strHash (const char *str)
{
    size_t hash = 0;
    const size_t len = wordLen (str);

    for (size_t i = 0; i < len; ++i)
        hash += 33 * str[i];

    return hash % NHASH;
}

bool isLabel (const char *str)
{   
    const size_t len = wordLen (str);

    if (str[len - 1] != ':') return false;

    for (size_t i = 0; i < len - 1; ++i)
        if (!isalpha (str[i]) && str[i] != '_')
            return false;

    return true;
}