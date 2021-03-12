
#include "text.h"

static Token EMPTY_TOKEN = {NULL, 0};

size_t Text :: tokenizeText (const char *separator, const char *no_separator_fields, TOKEN_FORMAT format) 
{
    assert (separator); 
    assert (no_separator_fields);

    for (auto tok = getToken (separator, no_separator_fields); tok.str; tok = getToken (separator, no_separator_fields))
    {    
        tokens.push_back (tok);
        if (format == NULL_TERMINATED)
            tok.str [tok.size] = '\0';

        TEXT_LISTING ("get token %.*s", tok.size, tok.str)
    }
    return tokens.size();
}

Token Text :: getToken (const char *separator, const char *no_separator_fields)
{
    assert (separator);
    assert (no_separator_fields);

    Token tok = {};

    position += strspn (position, separator);

    const char *cur_field = strchr (no_separator_fields, *position);
    if (cur_field)
        tok = noSeparatorFieldProcessing (separator, position, cur_field);

    else if (*(position) == '\0') 
        tok = EMPTY_TOKEN;

    else
    {
        tok.size = strcspn (position, separator);
        tok.str = position;
    }

    position += tok.size;
    if (*position)
        ++position;

    return tok;
}

Token noSeparatorFieldProcessing (const char *separator, char *position, const char *field)
{
    assert (position);
    assert (field);

    Token tok = {};
    const char *end = strchr (position, field [2]);
    while (!strchr (separator, *end)) ++end;

    if (!end)
        tok = EMPTY_TOKEN;

    else if (field [1] == '#')
    {
            tok.str  = position;
            tok.size = end - position;
    }
    else if (field [1] == '-')
    {
        tok.str  = position + 1;
        tok.size = end - position - 1;
    }     

    return tok;
}

Token *Text :: getNextToken (Token *tok)
{
    if (tok == NULL) return &tokens [0];

    if (tok == &tokens [tokens.size() - 1])
        return NULL;

    TEXT_LISTING ("prev token \'%.*s\' next token \'%.*s\'", tok->size, tok->str, (tok + 1)->size, (tok + 1)->str)

    return ++tok;
}

Token *Text :: getLastLineToken (Token *tok)
{
    if (tok == &tokens [tokens.size() - 1] || !tok)
        return tok;

    size_t nline = getLineNumber (tok);
    while (++tok)
        if (getLineNumber (tok) > nline)
            return --tok;
    
    return NULL;        
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
    assert (file); // NULL log_file means no log

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

size_t fileSize (FILE *file)
{
    assert (file);
    struct stat file_info = {};
    fstat (fileno (file), &file_info);

    return file_info.st_size;
}