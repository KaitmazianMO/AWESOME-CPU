#ifndef TEXT_H
#define TEXT_H

#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <algorithm>
#include <iterator>

#include "listing.h" 


enum TOKEN_FORMAT { NULL_TERMINATED = 1, NO_FORMATNG = 2 };
enum ERRORS       {TXT_NULL_FILE_NAME, TXT_CANT_OPEN_THE_FILE, TXT_CANT_ALLOCATE_MEMORY,
                   TXT_NULL_TOKEN_PTR, TXT_NULL_BUFFER_PTR, TXT_NULL_SEPARATOR_PTR,
                   SYNTAX_ERROR};

class NonCopyable
{
    NonCopyable (const NonCopyable&) = delete;
    NonCopyable& operator = (const NonCopyable&) = delete; 

public:
    NonCopyable() 
    {}
};

struct Token 
{
    char *str;
    size_t size;
};

class Text
{
public:
    Text (const char *file, FILE *log_file = NULL);
   ~Text ();

    //! no separator fields format: "(#) [-]" #-uncluded brackets, [-] - not included
    //!                                       (aba) -> (aba)       [aba] -> aba 
    size_t tokenizeText (const char *separator, const char *no_separator_fields = "", TOKEN_FORMAT format = NO_FORMATNG);
    Token  getToken (const char *separator, const char *no_separator_fields = "");
    size_t getLineNumber (const Token *tok);
    Token *getNextToken (Token *tok);
    Token *getLastLineToken (Token *tok);   
    void fillStringsAfter (char after, char by);
    void setLogFile (FILE *log_) { log = log_; };
    const char *getFileName () { return name; };


private:
    char *text_buf;
    char *position;
    size_t buf_size;
    const char *name;
    std :: vector <char *> end_lines;
    std :: vector <Token> tokens;
    int error;
    FILE *log;
};

Token noSeparatorFieldProcessing (const char *separator, char *position, const char *field);
size_t fileSize (FILE *file);
#define wordLen( str )  ( strcspn (str, " \n\r\t\0") )

#endif