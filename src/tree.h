#include "parser.h"

enum NodeType
{
    D_TYPE,       // new func
    DECL_TYPE,    // function/variable declaration
    ID_TYPE,      // variabrle

    COMP_TYPE,    // {}
    STAT_TYPE,    // ';' or loop/condition

    COND_TYPE,    // condition
    LOOP_TYPE,    // while
    ASSG_TYPE,    // '='

    CALL_TYPE,    // call func
    JUMP_TYPE,    // return

    MATH_TYPE,    // + - * / < > <= >= == !=
    NUMB_TYPE,    // double

    TYPES_COUNT   // We will add scan print round
};

enum NodeValues
{
    FICT,
    WHILE,
    COMPOUND,
    STATEMENT,
    IF_ELSE,
    CALL
};

struct Node
{
    NodeType type;
    Value value;

    Node* parent;

    Node* left;
    Node* right;
};

struct Tree
{
    Node* root;
};

Tree* GetTree             (Parser* parser);
Node* GetData             (Parser* parser, size_t* ofs);
Node* GetDefinition       (Parser* parser, size_t* ofs);
Node* GetCompound         (Parser* parser, size_t* ofs);
Node* GetStatement        (Parser* parser, size_t* ofs);
Node* GetAssignment       (Parser* parser, size_t* ofs);
Node* GetExpression       (Parser* parser, size_t* ofs);
Node* GetSimpleExpression (Parser* parser, size_t* ofs);
Node* GetTerm             (Parser* parser, size_t* ofs);
Node* GetPrimaryExpression(Parser* parser, size_t* ofs);
Node* GetJump             (Parser* parser, size_t* ofs);
void  TreeDump            (Tree* tree);
