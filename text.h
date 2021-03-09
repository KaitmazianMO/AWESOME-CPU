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


using namespace std;

enum TOKEN_FORMAT { NULL_TERMINATED = 1, NO_FORMATNG = 2 };
enum ERRORS       {TXT_NULL_FILE_NAME, TXT_CANT_OPEN_THE_FILE, TXT_CANT_ALLOCATE_MEMORY,
                   TXT_NULL_TOKEN_PTR, TXT_NULL_BUFFER_PTR, TXT_NULL_SEPARATOR_PTR,
                   SYNTAX_ERROR};

struct Token 
{
    char *str;
    size_t size;
};

struct Text
{
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

    char *text_buf;
    char *position;
    size_t buf_size;
    const char *name;
    vector <char *> end_lines;
    vector <Token> tokens;
    int error;
    FILE *log;
};

Token noSeparatorFieldProcess (char *position, const char *field);
size_t fileSize (FILE *file);
#define wordLen( str )  ( strcspn (str, " \n\r\t\0") )

#endif
