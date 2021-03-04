#include "buffer.h"


Buffer *newBuffer (const char *file_name)
    {
    CATCH (!file_name, NULL_FILE_NAME_PTR) 

    Buffer *buffer = (Buffer *)calloc (1, sizeof (*buffer));
    CATCH (!buffer, NULL_BUFFER_PTR);

    FILE *file = fopen (file_name, "rb");
    CATCH (!file, NULL_FILE_PTR)

    size_t file_size = fileSize (file);

    buffer->data = (buff_t *)calloc (file_size + 1, sizeof (buff_t));
    CATCH (!buffer->data , NULL_BUFFER_DATA_PTR)
   
    buffer->size = fread (buffer->data, sizeof (buff_t), file_size, file);
    fclose (file);

    return buffer;
    }
 
char *findWord (char *str)
{
    size_t i = 0;
    while (isspace (str[i])) ++i;

    if (str[i] == '\0') return NULL;

    return str + i;
}

Buffer *dellBuffer (Buffer *buffer)
    {
    VERIFY_BUFFER

    free (buffer->data);
    free (buffer);

    return (Buffer *)NULL;
    }

size_t fileSize (FILE *file)
    {
    CATCH (!file, NULL_FILE_PTR)

    struct stat file_info = {};
    int stat_returned =  fstat (fileno (file), &file_info);
    CATCH (stat_returned, FSTAT_RETURNED_NOT_NULL);

    return file_info.st_size;
    }
                   