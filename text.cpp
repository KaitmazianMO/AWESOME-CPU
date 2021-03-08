
#include "text.h"

static size_t fileSize (FILE *file);
static Token EMPTY_TOKEN = {NULL, 0};


/*
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  @ todo modificate tokenize text
  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

struct Field
{
    const char begin; 
    const char end;
    const char *separators;

    Field (const char beg_, const char end_, const char * const sep_) 
        : begin (beg_), end (end_), separators (sep_)
        {};
};

//! separator format: "[some delim for all text] + %[symbols]:[special delims after symbols]"
//! example: " \t_%[]:\n" - devides text by ' ''\t' but if finds '[' devides by '\n' until find ']' (_ - is space)
size_t Text :: tokenizeText (const char *separator, TOKEN_FORMAT format)  
{
    assert (separator); 

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

    char *separator_format = (char *)calloc (strlen (separator) + 1, sizeof (char));
    assert (separator_format);
    strncpy (separator_format, separator, strlen (separator));

    std :: vector <Field> fields_vec;
    char *fields = strchr (separator_format, '\%') + 1;
    if (fields && fields > separator)
    {
        fields [-1] = '\0';

        const char *fields_separator = strchr (fields, ':');
        if (!fields_separator) {error = SYNTAX_ERROR; return EMPTY_TOKEN;}

        for (; fields < fields_separator; fields += 2)
            fields_vec.push_back (Field (fields[0], fields[1], fields_separator + 1));   
    }

    position += strspn (position, separator_format);
    char *tok_str = position;
    if (*(position) == '\0') return EMPTY_TOKEN;

    auto cur_field = std :: find_if (std :: begin (fields_vec), 
                                     std :: end (fields_vec), 
                                     [tok_str](Field fld)
                                     { 
                                         return (fld.begin == *tok_str);
                                     });

    size_t tok_size = 0;
    if (cur_field == std :: end (fields_vec))
        tok_size = strcspn (position, separator_format);
    else
        tok_size = strcspn (position, cur_field->separators);
    
    position += tok_size;
    if (*position != '\0')
        ++position;

    free (separator_format);
    return {tok_str, tok_size};
}

Token *Text :: getNextToken (Token *tok)
{
    if (tok == &tokens [tokens.size() - 1] || !tok)
        return NULL;

    TEXT_LISTING ("prev token \'%.*s\' next token \'%.*s\'", tok->size, tok->str, (tok + 1)->size, (tok + 1)->str)

    return ++tok;
}

Token *Text :: getLastLineToken (Token *tok)
{
    if (tok == &tokens [tokens.size() - 1] || !tok)
        return NULL;

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

static size_t fileSize (FILE *file)
{
    assert (file);
    struct stat file_info = {};
    fstat (fileno (file), &file_info);

    return file_info.st_size;
}
