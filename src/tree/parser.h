#pragma once

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const size_t MAX_SIZE = 1024;
static const size_t MAX_NAME_LEN = 32;

enum OperatorType
{
    DECLARATOR,
    LOOP,
    RET,
    IF,
    ELSE,
    EQUAL,
    ASSG,
    PLUS,
    MINUS,
    MUL,
    DIV,
    EABOVE,
    EBELOW,
    ABOVE,
    BELOW,
    NEQUAL,
    BRACE1,
    BRACE2,
    RNDBR1,
    RNDBR2,
    SMCLN,
    COMMA,

    NUM_OPERATORS
};

struct Operator
{
    const char* name;
    size_t len;
};

static const Operator OPERATORS[]
{
    { "double",  6 },
    { "while" ,  5 },
    { "return",  6 },
    { "if"    ,  2 },
    { "else"  ,  4 },
    { "=="    ,  2 },
    { "="     ,  1 },
    { "+"     ,  1 },
    { "-"     ,  1 },
    { "*"     ,  1 },
    { "/"     ,  1 },
    { "<="    ,  2 },
    { ">="    ,  2 },
    { "<"     ,  1 },
    { ">"     ,  1 },
    { "!="    ,  2 },
    { "{"     ,  1 },
    { "}"     ,  1 },
    { "("     ,  1 },
    { ")"     ,  1 },
    { ";"     ,  1 },
    { ","     ,  1 }
};

enum TokenType
{
    TYPE_NUMB,
    TYPE_ID,
    TYPE_OP
};

union Value
{
    int op;
    double number;
    char* name;
};

struct Token
{
    TokenType type;
    Value value;

    char* token_str;
};

enum ERROR
{
    BUFFER_IS_OK,
    TYPE_ERROR,
    MISSED_BRACE,
    MISSED_SMCLN,
    GET_NUMBER_ERR,
    BRACKET_ERR,
    UNKNOWN_SYMBOL,
    ASSG_ERROR
};

struct Buffer
{
    char* str;
    char* original_str;

    size_t lenght;
};

struct Parser
{
    Token* tokens;
    size_t size;

    char* original_buffer;
    ERROR status;
};


Parser* Parse           (const char* input);
void    ParserDump      (Parser* parser);
void    CompilationError(Parser* parser, size_t* idx);
Buffer* GetBuffer       (const char* input);
void    IgnoreSpaces    (Buffer* buffer);
