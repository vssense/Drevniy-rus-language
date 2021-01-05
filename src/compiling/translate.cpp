#include "compiling.h"

const double ZERO = 1e-5;
const int RANK = 10;

void TranslateTree      (Tree* tree, FILE* file);
void TranslateFunc      (Node* node, FILE* file);
void TranslateArgs      (Node* node, FILE* file);
void TranslateCompound  (Node* node, FILE* file, size_t num_tabs);
void TranslateStatement (Node* node, FILE* file, size_t num_tabs);
void TranslateLoop      (Node* node, FILE* file, size_t num_tabs);
void TranslateCondition (Node* node, FILE* file, size_t num_tabs);
bool TranslateStdCall   (Node* node, FILE* file);
void TranslateExpression(Node* node, FILE* file);
void TranslateParams    (Node* node, FILE* file);
void DoubleToDrevniyRus (FILE* file, double number);
bool Equal              (double num1, int num2);


void Translate(const char* file)
{
    assert(file);

    Tree* tree = GetFromFile(file);

    FILE* translate = fopen("translated_program.txt", "w");
    assert(translate);

    fprintf(translate, "~ THIS FILE WAS GENERATED AUTOMATICLY\n"
                       "~ TREE TAKED FROM FILE '%s'          \n"
                       "~ CHECK FILE BEFORE COMPILING      \n\n", file);

    TranslateTree(tree, translate);
}

void TranslateTree(Tree* tree, FILE* file)
{
    assert(tree);
    assert(file);

    Node* node = tree->root;

    while (node != nullptr)
    {
        TranslateFunc(node->right, file);
        node = node->left;
        fprintf(file, "\n");
    }
}

void TranslateFunc(Node* node, FILE* file)
{
    assert(node);
    assert(file);

    fprintf(file, "%s %s(", OPERATORS[DECLARATOR].name, node->value.name);
    TranslateArgs(node->right, file);
    fprintf(file, ")\n");
    TranslateCompound(node->left, file, 0);
}

void TranslateArgs(Node* node, FILE* file)
{
    assert(file);

    if (node != nullptr)
    {
        fprintf(file, "%s", node->value.name);
        node = node->right;

        while (node != nullptr)
        {
            fprintf(file, ", %s", node->value.name);
            node = node->right;
        }
    }
}

void SetTabs(FILE* file, size_t number)
{
    assert(file);

    for (size_t i = 0; i < number; ++i)
    {
        fprintf(file, "\t");
    }
}

void TranslateCompound(Node* node, FILE* file, size_t num_tabs)
{
    assert(node);
    assert(file);

    if (node == nullptr)
    {
        return;
    }

    node = node->right;
    SetTabs(file, num_tabs);
    fprintf(file, "%s\n", OPERATORS[BRACE1].name);

    while (node != nullptr)
    {
        TranslateStatement(node->left, file, num_tabs + 1);
        node = node->right;
    }

    SetTabs(file, num_tabs);
    fprintf(file, "%s\n", OPERATORS[BRACE2].name);
}

void TranslateStatement(Node* node, FILE* file, size_t num_tabs)
{
    assert(node);
    assert(file);

    SetTabs(file, num_tabs);

    switch (node->type)
    {
        case ASSG_TYPE :
        {
            fprintf(file, "%s %s ", node->left->value.name, OPERATORS[ASSG].name);
            TranslateExpression(node->right, file);
            fprintf(file, ";\n");
            break;
        }
        case DECL_TYPE :
        {
            fprintf(file, "%s %s %s ", OPERATORS[DECLARATOR].name, node->left->value.name, OPERATORS[ASSG].name);
            TranslateExpression(node->right, file);
            fprintf(file, ";\n");
            break;
        }
        case COND_TYPE :
        {
            TranslateCondition(node, file, num_tabs);
            break;
        }
        case LOOP_TYPE :
        {
            TranslateLoop(node, file, num_tabs);
            break;
        }
        case CALL_TYPE :
        {
            if (!TranslateStdCall(node, file))
            {
                fprintf(file, "%s(", node->left->value.name);
                TranslateParams(node->right, file);
                fprintf(file, ");\n");
            }
            break;
        }
        case JUMP_TYPE :
        {
            fprintf(file, "%s ", OPERATORS[RET].name);
            TranslateExpression(node->right, file);
            fprintf(file, ";\n");
            break;
        }
        default :
        {
            printf("I'm here, but I shouldn't, line = %d\n", __LINE__);
            break;
        }
    }
}

bool TranslateStdCall(Node* node, FILE* file)
{
    assert(node);
    assert(file);

    if (strcmp(node->left->value.name, "scan") == 0)
    {
        fprintf(file, "%s()", SCAN);
        return true;
    }

    if (strcmp(node->left->value.name, "print") == 0)
    {
        fprintf(file, "%s(", PRINT);
        TranslateParams(node->right, file);
        fprintf(file, ");\n");
        return true;
    }

    if (strcmp(node->left->value.name, "sqrt") == 0)
    {
        fprintf(file, "%s(", SQRT);
        TranslateParams(node->right, file);
        fprintf(file, ");\n");
        return true;
    }

    if (strcmp(node->left->value.name, "floor") == 0)
    {
        fprintf(file, "%s(", FLOOR);
        TranslateParams(node->right, file);
        fprintf(file, ");\n");
        return true;
    }

    return false;
}

void TranslateLoop(Node* node, FILE* file, size_t num_tabs)
{
    assert(node);
    assert(file);

    fprintf(file, "%s (", OPERATORS[LOOP].name);
    TranslateExpression(node->left, file);
    fprintf(file, ")\n");

    TranslateCompound(node->right, file, num_tabs);
}

void TranslateCondition(Node* node, FILE* file, size_t num_tabs)
{
    assert(node);
    assert(file);

    fprintf(file, "%s (", OPERATORS[IF].name);
    TranslateExpression(node->left, file);
    fprintf(file, ")\n");

    TranslateCompound(node->right->left, file, num_tabs);

    if (node->right->right != nullptr)
    {
        SetTabs(file, num_tabs);
        fprintf(file, "%s\n", OPERATORS[ELSE].name);
        TranslateCompound(node->right->right, file, num_tabs);
    }
}

bool NeededBracket(Node* node)
{
    assert(node);

    return (node->value.op == SUB_OP || node->value.op == ADD_OP) && node->parent->type == MATH_TYPE;
}

void TranslateExpression(Node* node, FILE* file)
{
    assert(node);
    assert(file);

    switch (node->type)
    {
        case MATH_TYPE :
        {
            if (NeededBracket(node))
            {
                fprintf(file, "(");
            }
            
            TranslateExpression(node->left, file);
            fprintf(file, " %s ", MathToString(node->value.op));
            TranslateExpression(node->right, file);

            if (NeededBracket(node)) 
            {
                fprintf(file, ")");
            }
            break;
        }
        case NUMB_TYPE :
        {   
            DoubleToDrevniyRus(file, node->value.number);   
            break;
        }
        case CALL_TYPE :
        {
            if (!TranslateStdCall(node, file))
            {
                fprintf(file, "%s(", node->left->value.name);
                TranslateParams(node->right, file);
                fprintf(file, ")");
            }
            break;
        }
        case ID_TYPE :
        {
            fprintf(file, "%s", node->value.name);
            break;
        }
        default :
        {
            printf("I'm here, but I shouldn't, line = %d\n", __LINE__);
            break;
        }
    }
}

void TranslateParams(Node* node, FILE* file)
{
    assert(file);

    if (node != nullptr)
    {

        while (node->right != nullptr)
        {
            node = node->right;
        }

        TranslateExpression(node->left, file);
        node = node->parent;

        while (node->type == ARG_TYPE)
        {
            fprintf(file, ", ");
            TranslateExpression(node->left, file);
            node = node->parent;
        }
    }
}

void DoubleToDrevniyRus(FILE* file, double number)
{
    assert(file);

    if (Equal(number, 0))
    {
        fprintf(file, "ноль");
        return;
    }

    bool NeededBracket = false;

    if (number < 0)
    {
        fprintf(file, "(целковый - полушка) * ");
        number *= -1;
        NeededBracket = true;
    }

    size_t decimal_place = 0;

    while (!Equal(number, (int)number))
    {
        number *= RANK;
        decimal_place++;
        NeededBracket = true;
    }

    int value = (int)number;

    size_t RankPower = 0;

    if (NeededBracket)
    {
        fprintf(file, "(");
    }

    while (value > 0)
    {
        int digit = value % RANK;
        fprintf(file, "%s", DIGITS[digit].digit);

        for (size_t i = 0; i < RankPower; ++i)
        {
            fprintf(file, " * десятичок");
        }

        if (value > RANK)
        {
            fprintf(file, " + ");
        }

        value /= RANK;
    }

    if (NeededBracket)
    {
        fprintf(file, ")");
    }

    if (decimal_place)
    {
        fprintf(file, " / десятичок");
    }

    for (size_t i = 1; i < decimal_place; ++i)
    {
        fprintf(file, " * десятичок");
    }
}


bool Equal(double num1, int num2)
{
    if ((num1 - num2) < ZERO)
    {
        return true;
    }

    return false;
}