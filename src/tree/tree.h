#include "parser.h"

enum NodeType
{
    D_TYPE,       // new func                         |   fict
    DECL_TYPE,    // variable declaration             |   var name
    ID_TYPE,      // variable or function             |   name of func or var    
    ARG_TYPE,     // bunch of arguments               |   args

    COMP_TYPE,    // {                                |   comp
    STAT_TYPE,    // ';' or loop/condition            |   S

    COND_TYPE,    // condition                        |   if
    IFEL_TYPE,    // if-else                          |   if-else
    LOOP_TYPE,    // while                            |   while
    ASSG_TYPE,    // '='                              |   =

    CALL_TYPE,    // call func                        |   call
    JUMP_TYPE,    // return                           |   return

    MATH_TYPE,    // + - * / < > <= >= == !=          |
    NUMB_TYPE,    // double                           |   *value of number*

    TYPES_COUNT   // We will add scan, print, round   |   p.s. Fictive nodes's values == 0
};

enum MathOp
{
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,

    EQUAL_OP,
    NOT_EQUAL_OP,
    LESS_OP,
    GREATER_OP,
    LESS_EQUAL_OP,
    GREATER_EQUAL_OP
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

Tree*       GetTree     (Parser* parser);
void        TreeDump    (Tree* tree);
void        WriteToFile (Tree* tree, const char* output);
Tree*       GetFromFile (const char* file);
const char* MathToString(int op);
void        DestructTree(Tree* tree);
void        DeleteTree  (Tree* tree);
