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
    EBELOW,
    EABOVE,
    BELOW,
    ABOVE,
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
    { "�������"           ,  7  },
    { "������������_����" ,  17 },
    { "�������"           ,  7  },
    { "�_������"          ,  8  },
    { "�����"             ,  5  },
    { "=="                ,  2  },
    { "���������"         ,  9  },
    { "+"                 ,  1  },
    { "-"                 ,  1  },
    { "*"                 ,  1  },
    { "/"                 ,  1  },
    { "<="                ,  2  },
    { ">="                ,  2  },
    { "<"                 ,  1  },
    { ">"                 ,  1  },
    { "!="                ,  2  },
    { "������"            ,  6  },
    { "��������"          ,  8  },
    { "("                 ,  1  },
    { ")"                 ,  1  },
    { ";"                 ,  1  },
    { ","                 ,  1  }
};

struct Digit
{
    const char* digit;
    size_t len;
};

static const Digit DIGITS[] =
{
    { "����"       , 4  }, 
    { "��������"   , 8  }, 
    { "�������"    , 7  }, 
    { "�����������", 11 }, 
    { "��������"   , 8  }, 
    { "���������"  , 9  }, 
    { "�������"    , 7  }, 
    { "����������" , 10 }, 
    { "����������" , 10 }, 
    { "���������"  , 9  },
    { "���������"  , 9  } 
};

const int NUM_DIGITS = 11;

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
void    DestructParser  (Parser* parser);
void    DeleteParser    (Parser* parser);
