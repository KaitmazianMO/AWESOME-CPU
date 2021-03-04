
#include "text.h"

static size_t fileSize (FILE *file);
static Token EMPTY_TOKEN = {NULL, 0};


size_t Text :: tokenizeText (const char *separator, TOKEN_FORMAT format)
{
    for (auto tok = getToken (separator); tok.str; tok = getToken (separator))
    {    
        tokens.push_back (tok);
        if (format == NULL_TERMINATED)
            tok.str [tok.size] = '\0';

        TEXT_LISTING ("get token %.*s", tok.size, tok.str)
    }
    return tokens.size();
}

Token Text :: getToken (const char *separator)
{
    assert (separator);

    position += strspn (position, separator);
    char *tok_str = position;
    if (*(position) == '\0') return EMPTY_TOKEN;

    size_t tok_size = strcspn (position, separator);
    position += tok_size;
    if (*position != '\0')
        ++position;

    return {tok_str, tok_size};
}

Token Text :: getNextToken()
{
    static size_t i = 0;
    static const size_t tsize = tokens.size();
    while (i < tsize)
        return tokens[i++];

    TEXT_LISTING ("get Token %.*s", tokens[i].size, tokens[i].str)
    i = 0;
    return EMPTY_TOKEN;
}

void Text :: fillStringsAfter (char after, char by)
{
    char *start = text_buf;
    for (auto com = strchr (start, after); com; com = strchr (com, after))
        for (; *com != '\n' && *com != '\0'; ++com)
            *com = by;
}

size_t Text :: getLineNumber (const Token *tok)
{
    if (!tok) { error = TXT_NULL_TOKEN_PTR; return 0; } 

    for (size_t i = 0; i < end_lines.size(); ++i)
        if (tok->str < end_lines [i])
            return i + 1;

    return -1;
}

Text :: Text (const char *file, FILE *log_file)
{
    name = file;
    log = log_file;

    if (!file) { error = TXT_NULL_FILE_NAME; return; }
    FILE *file_ptr = fopen (file, "rb");
    if (!file_ptr) { error = TXT_CANT_OPEN_THE_FILE; return;}

    size_t file_size = fileSize (file_ptr);

    text_buf = (char *)calloc (file_size + 1, sizeof (text_buf[0]));
    position = text_buf;
    if (!text_buf) { error = TXT_CANT_ALLOCATE_MEMORY; }
   
    buf_size = fread (text_buf, sizeof (text_buf[0]), file_size, file_ptr);
    fclose (file_ptr);

    for (char *endl = strchr (text_buf, '\n'); endl - text_buf < buf_size; endl = strchr (endl + 1, '\n'))
        end_lines.push_back (endl);
    end_lines.push_back (text_buf + buf_size);
}

Text :: ~Text ()
{
    free (text_buf);
    name = position = text_buf = NULL;
    buf_size = 0;

}

static size_t fileSize (FILE *file)
{
    assert (file);
    struct stat file_info = {};
    fstat (fileno (file), &file_info);

    return file_info.st_size;
}