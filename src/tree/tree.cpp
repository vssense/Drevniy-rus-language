#include "tree.h"
#include "dot_dump.h"

////////////////////////////////////////////////////////
// Building tree
////////////////////////////////////////////////////////
Node* GetData             (Parser* parser, size_t* ofs);
Node* GetDefinition       (Parser* parser, size_t* ofs);
Node* GetCompound         (Parser* parser, size_t* ofs);
Node* GetStatement        (Parser* parser, size_t* ofs);
Node* GetAssignment       (Parser* parser, size_t* ofs);
Node* GetExpression       (Parser* parser, size_t* ofs);
Node* GetSimpleExpression (Parser* parser, size_t* ofs);
Node* GetTerm             (Parser* parser, size_t* ofs);
Node* GetPrimaryExpression(Parser* parser, size_t* ofs);
Node* GetLoop             (Parser* parser, size_t* ofs);
Node* GetCondition        (Parser* parser, size_t* ofs);
Node* GetJump             (Parser* parser, size_t* ofs);
Node* GetArgs             (Parser* parser, size_t* ofs);
Node* GetParams           (Parser* parser, size_t* ofs);

void  CheckOpeningBrace   (Parser* parser, size_t* ofs);
void  CheckClosingBrace   (Parser* parser, size_t* ofs);
void  CheckOpeningBracked (Parser* parser, size_t* ofs);
void  CheckClosingBracked (Parser* parser, size_t* ofs);
void  CheckSemicolon      (Parser* parser, size_t* ofs);
void  CheckAssigment      (Parser* parser, size_t* ofs);
bool  IsComparison        (Parser* parser, size_t* ofs);

Tree* NewTree      ();
Node* NewNode      ();
Node* ConstructNode(NodeType type, Value value, Node* left, Node* right);
Node* ConstructNode(NodeType type, Node* left, Node* right);
Node* ConstructNode(NodeType type, Value value);

///////////////////////////////////////////////////////
// Work with file
///////////////////////////////////////////////////////
void WriteToFileRecursively(Node* node, FILE* file);
void GetTreeFromBuffer     (Node* node, Buffer* buffer);
void DeleteFictiveNodes    (Node* node);


Tree* NewTree()
{
    Tree* tmp = (Tree*)calloc(1, sizeof(Tree));
    assert(tmp);

    return tmp;
}

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

Node* ConstructNode(NodeType type, Node* left, Node* right)
{
    return ConstructNode(type, { .op = 0 }, left, right);
}

Node* ConstructNode(NodeType type, Value value)
{
    ConstructNode(type, value, nullptr, nullptr);
}


#define TYPE       parser->tokens[*ofs].type
#define VALUE      parser->tokens[*ofs].value
#define NEXT_TYPE  parser->tokens[(*ofs) + 1].type
#define NEXT_VALUE parser->tokens[(*ofs) + 1].value
#define ID(left, right) ConstructNode(ID_TYPE, { .name = parser->tokens[(*ofs)++].value.name }, left, right)

Tree* GetTree(Parser* parser)
{
    assert(parser);

    Tree* tree = NewTree();

    size_t ofs = 0;

    tree->root = GetData(parser, &ofs);

    return tree;
}

Node* GetData(Parser* parser, size_t* ofs)
{
    assert(parser);

    Node* result = ConstructNode(D_TYPE, nullptr, GetDefinition(parser, ofs));
    Node* last = result;

    while (*ofs < parser->size)
    {
        last->left = ConstructNode(D_TYPE, nullptr, GetDefinition(parser, ofs));

        last->left->parent = last;
        last = last->left;
    }

    return result;
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

    Node* tmp = ID(nullptr, nullptr);
    tmp->right = GetArgs(parser, ofs);
    tmp->left = GetCompound(parser, ofs);

    return tmp;
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
            (*ofs)++;

            last->right = ID(nullptr, nullptr);
            last->right->parent = last;
            last = last->right;
        }
    }

    CheckClosingBracked(parser, ofs);

    return result;
}

Node* GetCompound(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckOpeningBrace(parser, ofs);

    Node* result = ConstructNode(COMP_TYPE, { .op = BRACE1 });
    Node* last = result;

    while (TYPE != TYPE_OP || VALUE.op != BRACE2)
    {
        last->right = ConstructNode(STAT_TYPE, { .op = SMCLN }, GetStatement(parser, ofs), nullptr);
        last->right->parent = last;
        last = last->right;
    }
 
    CheckClosingBrace(parser, ofs);

    return result;
}

Node* GetStatement(Parser* parser, size_t* ofs)
{
    assert(parser);

    if (TYPE == TYPE_OP && VALUE.op == DECLARATOR) // Initialization
    {
        (*ofs)++;
        Node* tmp = GetAssignment(parser, ofs);
        tmp->left->type = DECL_TYPE;
        CheckSemicolon(parser, ofs);
        return tmp;
    }

    if (TYPE == TYPE_ID && NEXT_TYPE == TYPE_OP && NEXT_VALUE.op == ASSG) // assignment
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

    if (TYPE == TYPE_OP && VALUE.op == IF)
    {
        return GetCondition(parser, ofs);
    }

    if (TYPE == TYPE_OP && VALUE.op == LOOP)
    {
        return GetLoop(parser, ofs);
    }

    Node* tmp = GetExpression(parser, ofs);
    CheckSemicolon(parser, ofs);
    return tmp;
}

Node* GetAssignment(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckAssigment(parser, ofs);

    Node* var = ID(nullptr, nullptr);
    (*ofs)++; /* to skip '=' */

    return ConstructNode(ASSG_TYPE, { .op = ASSG }, var, GetExpression(parser, ofs));
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
            Node* tmp = ID(nullptr, nullptr);
            return ConstructNode(CALL_TYPE, tmp, GetParams(parser, ofs));             
        }
        else
        {
            return ID(nullptr, nullptr);
        }
    }

    double val = VALUE.number;
    (*ofs)++;

    return ConstructNode(NUMB_TYPE, { .number = val });
}

Node* GetJump(Parser* parser, size_t* ofs)
{
    assert(parser);

    (*ofs)++;

    return ConstructNode(JUMP_TYPE, { .op = RET }, nullptr, GetExpression(parser, ofs));
}

Node* GetParams(Parser* parser, size_t* ofs)
{
    assert(parser);

    CheckOpeningBracked(parser, ofs);

    Node* result = nullptr;

    if (TYPE != TYPE_OP || VALUE.op != RNDBR2)
    {
        result = ConstructNode(ARG_TYPE, GetExpression(parser, ofs), nullptr);

        while (TYPE == TYPE_OP && VALUE.op == COMMA)
        {
            (*ofs)++;

            result->parent = ConstructNode(ARG_TYPE, GetExpression(parser, ofs), nullptr);
            result->parent->right = result;
            result = result->parent;
        }
    }

    CheckClosingBracked(parser, ofs);

    return result;
}

Node* GetLoop(Parser* parser, size_t* ofs)
{
    assert(parser);

    (*ofs)++;

    CheckOpeningBracked(parser, ofs);
    Node* result = ConstructNode(LOOP_TYPE, { .op = LOOP }, GetExpression(parser, ofs), nullptr);
    CheckClosingBracked(parser, ofs);

    result->right = GetCompound(parser, ofs);
    result->right->parent = result;

    return result;
}

Node* GetCondition(Parser* parser, size_t* ofs)
{
    assert(parser);

    (*ofs)++;
    CheckOpeningBracked(parser, ofs);

    Node* result = ConstructNode(COND_TYPE, GetExpression(parser, ofs), nullptr);

    CheckClosingBracked(parser, ofs);

    result->right = ConstructNode(IFEL_TYPE, GetCompound(parser, ofs), nullptr);

    if (TYPE == TYPE_OP && VALUE.op == ELSE)
    {
        (*ofs)++;
        result->right->right = GetCompound(parser, ofs);
        result->right->right->parent = result->right;
    }

    return result;
}

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

bool IsComparison(Parser* parser, size_t* ofs)
{
    assert(parser);

    return TYPE == TYPE_OP && (VALUE.op == EQUAL || VALUE.op == NEQUAL ||
                               VALUE.op == ABOVE || VALUE.op == EABOVE ||
                               VALUE.op == BELOW || VALUE.op == EBELOW);
}

void WriteToFile(Tree* tree, const char* output)
{
    assert(tree);

    FILE* file = fopen(output, "w");
    assert(file);

    WriteToFileRecursively(tree->root, file);

    fclose(file);
}

void WriteToFileRecursively(Node* node, FILE* file)
{
    assert(file);

    if (node == nullptr)
    {
        return;
    }

    fprintf(file, "%d | ", node->type);

    if (node->type == NUMB_TYPE)
    {
        fprintf(file, "%lg ", node->value.number);
    }
    else if (node->type == ID_TYPE || node->type == DECL_TYPE)
    {
        fprintf(file, "%s ", node->value.name);
    }
    else 
    {
        fprintf(file, "%d ", node->value.op);
    }


    fprintf(file, "{ ");
    WriteToFileRecursively(node->left, file);
    fprintf(file, "} ");

    fprintf(file, "{ ");
    WriteToFileRecursively(node->right, file);
    fprintf(file, "} ");
}

Tree* GetFromFile(const char* file)
{
    assert(file);

    Buffer* buffer = GetBuffer(file);

    Tree* tree = NewTree();


    tree->root = ConstructNode(D_TYPE, { .op = 0 });
    buffer->str += 5;

    GetTreeFromBuffer(tree->root, buffer);

    DeleteFictiveNodes(tree->root);

    return tree;
}

void GetTreeFromBuffer(Node* node, Buffer* buffer)
{
    assert(node);
    assert(buffer);

    while (size_t(buffer->str - buffer->original_str) < buffer->lenght)
    {
        IgnoreSpaces(buffer);

        if (*buffer->str == '{')
        {
            if (node->left == nullptr)
            {
                node->left = NewNode();
                node->left->parent = node;
                node = node->left;
            }
            else
            {
                node->right = NewNode();
                node->right->parent = node;
                node = node->right;
            }

            buffer->str++;
            continue;
        }

        if (*buffer->str == '}')
        {
            node = node->parent;
            buffer->str++;
            continue;
        }

        int len = 0;
        sscanf(buffer->str, "%d %*c %n", &node->type, &len);
        buffer->str += len;

        if (node->type == NUMB_TYPE)
        {
            sscanf(buffer->str, "%lg %n", &node->value.number, &len);
        }
        else if (node->type == ID_TYPE || node->type == DECL_TYPE)
        {
            node->value.name = (char*)calloc(MAX_NAME_LEN, sizeof(char));
            assert(node->value.name);

            sscanf(buffer->str, "%s %n", node->value.name, &len);
        }
        else
        {
            sscanf(buffer->str, "%d %n", &node->value.op, &len);
        }

        buffer->str += len;
    }
}

void DeleteFictiveNodes(Node* node)
{
    assert(node);

    if (node->left == nullptr && node->right == nullptr)
    {
        if (node->parent->left == node)
        {
            node->parent->left = nullptr;
        }
        else
        {
            node->parent->right = nullptr;
        }

        free(node);
    }
    else
    {
        if (node->left != nullptr)
        {
            DeleteFictiveNodes(node->left);
        }

        if (node->right != nullptr)
        {
            DeleteFictiveNodes(node->right);
        }
    }
}

