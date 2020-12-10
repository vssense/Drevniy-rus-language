#include "tree.h"

Node* NewNode()
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    assert(node);

    node->left   = nullptr;
    node->right  = nullptr;
    node->parent = nullptr;

    return node; 
}

Node* ConstructNode(NodeType type, Value value, Node* left, Node* right)
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    assert(node);

    node->type  = type;
    node->value = value;

    node->left  = left;
    node->right = right;

    if (left != nullptr)
    {
        left->parent = node;
    }

    if (right != nullptr)
    {
        right->parent = node;
    }

    return node;
}

Tree* NewTree()
{
    Tree* tmp = (Tree*)calloc(1, sizeof(Tree));
    assert(tmp);

    return tmp;
}

#define TYPE       parser->tokens[*ofs].type
#define VALUE      parser->tokens[*ofs].value
#define NEXT_TYPE  parser->tokens[(*ofs) + 1].type
#define NEXT_VALUE parser->tokens[(*ofs) + 1].value
#define ID(left, right) ConstructNode(ID_TYPE, { .name = parser->tokens[(*ofs)++].value.name }, left, right)


void CheckOpeningBrace(Parser* parser, size_t* ofs)
{
    assert(parser);

    if (TYPE != TYPE_OP || VALUE.op != BRACE1)
    {
        parser->status = MISSED_BRACE;
        CompilationError(parser, ofs);
    }

    (*ofs)++;
}

void CheckClosingBrace(Parser* parser, size_t* ofs)
{
    assert(parser);
 
    if (TYPE != TYPE_OP || VALUE.op != BRACE2)
    {
        parser->status = MISSED_BRACE;
        CompilationError(parser, ofs);
    }

    (*ofs)++;
}

void CheckOpeningBracked(Parser* parser, size_t* ofs)
{
    assert(parser);

    if (TYPE != TYPE_OP || VALUE.op != RNDBR1)
    {
        parser->status = MISSED_BRACE;
        CompilationError(parser, ofs);
    }

    (*ofs)++;
}

void CheckClosingBracked(Parser* parser, size_t* ofs)
{
    assert(parser);
 
    if (TYPE != TYPE_OP || VALUE.op != RNDBR2)
    {
        parser->status = MISSED_BRACE;
        CompilationError(parser, ofs);
    }

    (*ofs)++;
}

void CheckSemicolon(Parser* parser, size_t* ofs)
{
    assert(parser);
 
    if (TYPE != TYPE_OP || VALUE.op != SMCLN)
    {
        parser->status = MISSED_SMCLN;
        CompilationError(parser, ofs);
    }

    (*ofs)++;
}

void CheckAssigment(Parser* parser, size_t* ofs)
{
    assert(parser);
 
    if (NEXT_TYPE != TYPE_OP || NEXT_VALUE.op != ASSG || TYPE != TYPE_ID)
    {
        parser->status = ASSG_ERROR;
        CompilationError(parser, ofs);
    }
}

Node* GetArgs(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckOpeningBracked(parser, ofs);

    Node* result = nullptr;

    if (TYPE == TYPE_ID)
    {
        result = ID(nullptr, nullptr);
        Node* last = result;   

        while (TYPE == TYPE_OP && VALUE.op == COMMA)
        {
            (*ofs++);
            last->left = ID(nullptr, nullptr);

            last->left->parent = last;
            last = last->left;
        }
    }

    CheckClosingBracked(parser, ofs);

    return result;
}

bool IsComparison(Parser* parser, size_t* ofs)
{
    assert(parser);

    return TYPE == TYPE_OP && (VALUE.op == EQUAL || VALUE.op == NEQUAL ||
                               VALUE.op == ABOVE || VALUE.op == EABOVE ||
                               VALUE.op == BELOW || VALUE.op == EBELOW);
}

Node* GetPrimaryExpression(Parser* parser, size_t* ofs)
{
    assert(parser);

    if (TYPE == TYPE_OP && VALUE.op == RNDBR1)
    {
        CheckOpeningBracked(parser, ofs);
        Node* tmp = GetExpression(parser, ofs);
        CheckClosingBracked(parser, ofs);

        return tmp;
    }

    if (TYPE == TYPE_ID)
    {
        if (NEXT_TYPE == TYPE_OP && NEXT_VALUE.op == RNDBR1) // call
        {
           return ConstructNode(CALL_TYPE, { .op = CALL }, ID(nullptr, nullptr), GetArgs(parser, ofs));             
        }                                // ^^~~~~~ change when create special type for call!!!
        else
        {
            return ID(nullptr, nullptr);
        }
    }

    double val = VALUE.number;
    (*ofs)++;

    return ConstructNode(NUMB_TYPE, { .op = val }, nullptr, nullptr);
}

Node* GetTerm(Parser* parser, size_t* ofs)
{
    assert(parser);

    Node* result = GetPrimaryExpression(parser, ofs);

    while (TYPE == TYPE_OP && (VALUE.op == MUL || VALUE.op == DIV))
    {
        Value op = VALUE;
        (*ofs)++;

        result = ConstructNode(MATH_TYPE, op, result, GetPrimaryExpression(parser, ofs));
    }

    return result;
}

Node* GetSimpleExpression(Parser* parser, size_t* ofs)
{
    assert(parser);

    Node* result = GetTerm(parser, ofs);

    while (TYPE == TYPE_OP && (VALUE.op == PLUS || VALUE.op == MINUS))
    {
        Value op = VALUE;
        (*ofs)++;

        result = ConstructNode(MATH_TYPE, op, result, GetTerm(parser, ofs));
    }

    return result;
}

Node* GetExpression(Parser* parser, size_t* ofs)
{
    assert(parser);

    Node* result = GetSimpleExpression(parser, ofs);

    while (IsComparison(parser, ofs))
    {
        Value op = VALUE;        
        (*ofs)++;

        result = ConstructNode(MATH_TYPE, op, result, GetSimpleExpression(parser, ofs));
    }

    return result;
}

Node* GetAssignment(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckAssigment(parser, ofs);

    Node* var = ID(nullptr, nullptr);
    (*ofs)++; /* to skip '=' */

    return ConstructNode(ASSG_TYPE, { .op = ASSG }, var, GetExpression(parser, ofs));
}

Node* GetJump(Parser* parser, size_t* ofs)
{
    assert(parser);

    (*ofs)++;

    return ConstructNode(JUMP_TYPE, { .op = RET }, GetExpression(parser, ofs), nullptr);
}

Node* GetStatement(Parser* parser, size_t* ofs)
{
    assert(parser);

    if (TYPE == TYPE_OP && VALUE.op == DECLARATOR) // Initialization
    {
        (*ofs)++;
        Node* tmp = GetAssignment(parser, ofs);
        CheckSemicolon(parser, ofs);
        return tmp;
    }

    if (TYPE == TYPE_ID && NEXT_TYPE == TYPE_OP && NEXT_VALUE.op == ASSG) // assignation
    {
        Node* tmp = GetAssignment(parser, ofs);
        CheckSemicolon(parser, ofs);
        return tmp;
    }

    if (TYPE == TYPE_OP && VALUE.op == RET) // return
    {
        Node* tmp = GetJump(parser, ofs);
        CheckSemicolon(parser, ofs);
        return tmp;
    }

    return GetExpression(parser, ofs);
}

Node* GetCompound(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckOpeningBrace(parser, ofs);

    Node* result = ConstructNode(COMP_TYPE, { .op = BRACE1 }, nullptr, nullptr);
    Node* last = result;

    while (TYPE != TYPE_OP || VALUE.op != BRACE2)
    {
        last->right = ConstructNode(COMP_TYPE, { .op = SMCLN }, GetStatement(parser, ofs), nullptr);
        last->right->parent = last;
        last = last->right;
    }
 
    CheckClosingBrace(parser, ofs);
}

Node* GetDefinition(Parser* parser, size_t* ofs)
{
    assert(parser);
    assert(ofs);

    if (TYPE != TYPE_OP || VALUE.op != DECLARATOR)
    {
        parser->status = TYPE_ERROR;
        CompilationError(parser, ofs);
    }

    (*ofs)++;

    char* name = VALUE.name;

    return ID(GetArgs(parser, ofs), GetCompound(parser, ofs));
}

Node* GetData(Parser* parser, size_t* ofs)
{
    assert(parser);

    Node* result = ConstructNode(D_TYPE, { .op = FICT }, nullptr, GetDefinition(parser, ofs));
    Node* last = result;

    while (*ofs < parser->size)
    {
        last->left = ConstructNode(D_TYPE, { .op = FICT }, nullptr, GetDefinition(parser, ofs));

        last->left->parent = last;
        last = last->left;
    }

    return result;
}

Tree* GetTree(Parser* parser)
{
    assert(parser);

    Tree* tree = NewTree();

    size_t ofs = 0;

    tree->root = GetData(parser, &ofs);

    return tree;
}

const char* STANDARD_DOT_TXT_FILE_NAME = "log\\tree.txt";

const size_t DOT_CMD_SIZE = 64;
const size_t JPG_CMD_SIZE = 32;
const char*  DUMP_FILE_NAME = "log\\numsjpg.txt";
const size_t NUM_STR_LEN = 5;
const size_t NOTATION = 10;

size_t GetJPGNumber()
{
    FILE* numjpgs = fopen(DUMP_FILE_NAME, "r");

    size_t num = 0;
    
    if (numjpgs != nullptr)
    {
        fscanf(numjpgs, "%llu", &num);
    }

    fclose(numjpgs);

    numjpgs = fopen(DUMP_FILE_NAME, "w");
    fprintf(numjpgs, "%llu", num + 1);
    fclose(numjpgs);
    
    return num;
}

void GetNames(char* dot_cmd, char* jpg_cmd)
{
    assert(dot_cmd);
    assert(jpg_cmd);

    size_t num = GetJPGNumber();

    char num_str[NUM_STR_LEN] = "";
    itoa(num, num_str, NOTATION);
    char extension[] = ".jpg";


    strcat(dot_cmd, num_str);
    strcat(dot_cmd, extension);
    
    strcat(jpg_cmd, num_str);
    strcat(jpg_cmd, extension);
}

void PrintNodes(Tree* tree, Node* node, FILE* dump_file)
{
    assert(tree);
    assert(dump_file);

    if (node->type == ID_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%s\"]",
               node, node->value.name);
    }
    else if (node->type == NUMB_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%lg\"]",
          node, node->value.number); 
    }
    else if (node->value.op != CALL)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%s\"]",
               node, OPERATORS[node->value.op].name);
    }
    else
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EF4C3A\", label=\"%s\"]",
               node, "CALL");
    
    }
}

void TreeDump(Tree* tree)
{
    assert(tree);

    FILE* dump_file = fopen(STANDARD_DOT_TXT_FILE_NAME, "w");

    fprintf(dump_file, "digraph G{\n");
    fprintf(dump_file, "node [shape=\"circle\"]\n");

    PrintNodes(tree, tree->root, dump_file);

    fprintf(dump_file, "}");

    fclose(dump_file);

    char dot_cmd[DOT_CMD_SIZE] = "";
    snprintf(dot_cmd, DOT_CMD_SIZE, "dot -Tjpg %s -o log\\Dump", STANDARD_DOT_TXT_FILE_NAME);
    char jpg_cmd[JPG_CMD_SIZE] = "start log\\Dump";
    
    GetNames(dot_cmd, jpg_cmd);

    system(dot_cmd);
    system(jpg_cmd);
}
