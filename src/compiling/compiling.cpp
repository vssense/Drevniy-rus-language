#include "compiling.h"

const char* ASM_FILE_NAME = "asm_tmp.txt";

/////////////////////////////////////////////////////////////////
//// NameTable
/////////////////////////////////////////////////////////////////
NameTable* MakeTableOfNames (Tree* tree);
size_t     GetNumFunc       (Tree* tree);
NameTable* NewNameTable     ();
void       FillNameTable    (NameTable* table, Tree* tree);
void       GetFunction      (Node* subtree, Function* function);
size_t     GetNumArgs       (Node* node);
size_t     GetNumVars       (Node* node);
void       GetNames         (char** vars, Node* subtree);
void       GetNewVars       (Node* node, char** vars, size_t* ofs);
void       DumpNameTable    (NameTable* table);
Function*  FindFunc         (const char* name, NameTable* table);
void       DestructNameTable(NameTable* table);
void       DeleteNameTable  (NameTable* table);

/////////////////////////////////////////////////////////////////
///// Assemble
/////////////////////////////////////////////////////////////////
void   WriteAsmCode             (Tree* tree, NameTable* table, FILE* file);
void   WriteAsmFunc             (Node* node, Compilier* compilier);
void   WriteAsmCompound         (Node* node, Compilier* compilier);
void   WriteAsmStatement        (Node* node, Compilier* compilier);
void   WriteAsmLoop             (Node* node, Compilier* compilier);
void   WriteAsmCondition        (Node* node, Compilier* compilier);
void   WriteAsmCall             (Node* node, Compilier* compilier);
bool   WriteAsmStdCall          (Node* node, Compilier* compilier);
void   WriteAsmAssignment       (Node* node, Compilier* compilier);
void   WriteAsmExpression       (Node* node, Compilier* compilier);
void   WriteAsmSimpleExpression (Node* node, Compilier* compilier);
void   WriteAsmPrimaryExpression(Node* node, Compilier* compilier);
void   WriteAsmCompare          (Node* node, Compilier* compilier);
size_t GetVarOfs                (Function* function, char* name);


#define ASM_ASSERT assert(node);               \
                   assert(compilier->table);   \
                   assert(compilier->file);    \
                   assert(compilier->function)

#define FUNC      compilier->function
#define ASM_FILE  compilier->file
#define LABEL     compilier->label

void Compile(const char* input)
{
    assert(input);

    Parser* parser = Parse(input);
    Tree* tree = GetTree(parser);
    Assemble(tree);

    DestructTree(tree);
    DeleteTree(tree);

    DestructParser(parser);    
    DeleteParser(parser);    
}

void Assemble(Tree* tree)
{
    assert(tree);

    NameTable* table = MakeTableOfNames(tree);

    FILE* asm_file = fopen(ASM_FILE_NAME, "w");
    assert(asm_file);

    WriteAsmCode(tree, table, asm_file);
 
    fclose(asm_file);
}

void WriteAsmCode(Tree* tree, NameTable* table, FILE* file)
{
    assert(tree);
    assert(file);

    Node* node = tree->root;

    Compilier compilier = {};
    compilier.table     = table;
    compilier.function  = table->functions;
    compilier.file      = file;
    compilier.label     = 0;

    fprintf(file, "call main\n"
                  "out      \n"
                  "stop   \n\n");

    for (size_t i = 0; i < table->num_func; ++i)
    {
        WriteAsmFunc(node->right, &compilier);

        compilier.function++;
        node = node->left;
    }

    DestructNameTable(table);
    DeleteNameTable(table);
}

void WriteAsmFunc(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    fprintf(ASM_FILE, "\n%s :   \n", node->value.name);
    fprintf(ASM_FILE, "push rax \n"
                      "pop rbx  \n"
                      "push rax \n"
                      "push rax \n"
                      "push %llu\n"
                      "sum      \n"
                      "pop rax  \n"
                      "pop [rax]\n", FUNC->num_args + FUNC->num_vars + 1);

    Node* tmp = node->right;
    for (size_t i = 0; i < FUNC->num_args; ++i)
    {
        fprintf(ASM_FILE, "pop [rbx + %llu]\n", GetVarOfs(FUNC, tmp->value.name));
        tmp = tmp->right;
    }


    WriteAsmCompound(node->left->right, compilier);
}

void WriteAsmCompound(Node* node, Compilier* compilier)
{
    if (node == nullptr)
    {
        return;
    }

    ASM_ASSERT;

    while (node != nullptr)
    {
        WriteAsmStatement(node->left, compilier);
        node = node->right;
    }
}

void WriteAsmStatement(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    switch (node->type)
    {
        case DECL_TYPE :
        {
            WriteAsmAssignment(node, compilier);
            break;
        }
        case ASSG_TYPE :
        {
            WriteAsmAssignment(node, compilier);
            break;
        }
        case JUMP_TYPE :
        {
            WriteAsmExpression(node->right, compilier);

            fprintf(ASM_FILE, "push [rax]\n"
                              "pop rax   \n"
                              "ret       \n");
            break;
        }
        case COND_TYPE :
        {
            WriteAsmCondition(node, compilier);
            break;
        }
        case LOOP_TYPE :
        {
            WriteAsmLoop(node, compilier);
            break;
        }
        case CALL_TYPE :
        {
            WriteAsmCall(node, compilier);
            break;
        }
        default :
        {
            printf("I am here, but I shouldn't, line = %d\n", __LINE__);
            break;
        }
    }
}

void WriteAsmLoop(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    size_t label = LABEL++;

    fprintf(ASM_FILE, "LOOP_%llu :\n", label);
    WriteAsmExpression(node->left, compilier);

    fprintf(ASM_FILE, "push 0          \n"
                      "je CONTINUE_%llu\n", label);

    WriteAsmCompound(node->right->right, compilier);

    fprintf(ASM_FILE, "jmp LOOP_%llu  \n"
                      "CONTINUE_%llu :\n", label, label);
}

void WriteAsmCondition(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    WriteAsmExpression(node->left, compilier);

    size_t label = LABEL++;

    fprintf(ASM_FILE, "push 0        \n"
                      "jne TRUE_%llu \n"
                      "jmp FALSE_%llu\n"
                      "TRUE_%llu :   \n", label, label, label);

    WriteAsmCompound(node->right->left->right, compilier);

    fprintf(ASM_FILE, "jmp CONTINUE_%llu\n"
                      "FALSE_%llu :     \n", label, label);

    if (node->right->right != nullptr)
    {
        WriteAsmCompound(node->right->right->right, compilier);
    }

    fprintf(ASM_FILE, "jmp CONTINUE_%llu\n"
                      "CONTINUE_%llu :  \n", label, label);
}

void WriteAsmAssignment(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    WriteAsmExpression(node->right, compilier);

    fprintf(ASM_FILE, "pop [rbx + %llu]\n", GetVarOfs(FUNC, node->left->value.name));
}

void WriteAsmExpression(Node* node, Compilier* compilier)
{
    if (node == nullptr)
    {
        return;    
    }

    ASM_ASSERT;

    if (node->type == MATH_TYPE && node->value.op >= EQUAL_OP) // Is comparison
    {
        WriteAsmExpression(node->left,  compilier);
        WriteAsmExpression(node->right, compilier);
        WriteAsmCompare(node, compilier);
    }
    else
    {
        WriteAsmSimpleExpression(node, compilier);
    }
}

void WriteAsmSimpleExpression(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    if (node->type == MATH_TYPE)
    {
        WriteAsmSimpleExpression(node->left,  compilier);
        WriteAsmSimpleExpression(node->right, compilier);
        
        switch (node->value.op)
        {
            case MUL_OP :
            {
                fprintf(ASM_FILE, "mul\n");
                break;
            }
            case DIV_OP :
            {   
                fprintf(ASM_FILE, "div\n");
                break;
            }
            case ADD_OP :
            {   
                fprintf(ASM_FILE, "sum\n");
                break;
            }
            case SUB_OP :
            {
                fprintf(ASM_FILE, "dec\n");
                break;
            }
            default:
            {
                printf("I am here, but I shouldn't, line = %d\n", __LINE__);
            }
        }
    }
    else
    {
        WriteAsmPrimaryExpression(node, compilier);
    }
}

void WriteAsmPrimaryExpression(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    switch (node->type)
    {
        case ID_TYPE :
        {
            fprintf(ASM_FILE, "push [rbx + %llu]\n", GetVarOfs(FUNC, node->value.name));
            break;
        }
        case NUMB_TYPE :
        {
            fprintf(ASM_FILE, "push %lf\n", node->value.number);
            break;
        }
        case CALL_TYPE :
        {
            WriteAsmCall(node, compilier);
            break;
        }
        default :
        {
            printf("I am here, but I shouldn't, line = %d\n", __LINE__);
        }
    }
}

void WriteAsmCall(Node* node, Compilier* compilier)
{
    if (WriteAsmStdCall(node, compilier))
    {
        return;
    }

    ASM_ASSERT;

    Function* call_func = FindFunc(node->left->value.name, compilier->table);

    if (call_func != nullptr)
    {       
        for (size_t i = 0; i < call_func->num_args; ++i)
        {
            if (node == nullptr)
            {
                printf("error : too few arguments for '%s'\n", call_func->name);
                break;
            }
            node = node->right;
            WriteAsmExpression(node->left, compilier);
        }

        if (node->right != nullptr)
        {
            printf("error : too more arguments for '%s'\n", call_func->name);
        }

        fprintf(ASM_FILE, "call %s\n", call_func->name);
    }
}

bool WriteAsmStdCall(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    if (strcmp(node->left->value.name, SCAN) == 0)
    {
        fprintf(ASM_FILE, "in\n");
        return true;
    }

    if (strcmp(node->left->value.name, PRINT) == 0)
    {
        WriteAsmExpression(node->right->left, compilier);
        fprintf(ASM_FILE, "out\n");
        return true;
    }

    if (strcmp(node->left->value.name, SQRT) == 0)
    {
        WriteAsmExpression(node->right->left, compilier);
        fprintf(ASM_FILE, "sqrt\n");
        return true;
    }

    if (strcmp(node->left->value.name, FLOOR) == 0)
    {
        WriteAsmExpression(node->right->left, compilier);
        fprintf(ASM_FILE, "round\n");
        return true;
    }

    return false;
}

Function* FindFunc(const char* name, NameTable* table)
{
    assert(name);

    for (size_t i = 0; i < table->num_func; ++i)
    {
        if (strcmp(table->functions[i].name, name) == 0)
        {
            return table->functions + i;
        }
    }

    printf("error : unknown function : '%s'\n", name);

    return nullptr;
}

void WriteAsmCompare(Node* node, Compilier* compilier)
{
    ASM_ASSERT;

    switch (node->value.op)
    {
        case EQUAL_OP :
        {
            fprintf(ASM_FILE, "je ");
            break;
        }
        case NOT_EQUAL_OP :
        {
            fprintf(ASM_FILE, "jne ");
            break;
        }
        case LESS_OP :
        {
            fprintf(ASM_FILE, "jb ");
            break;
        }
        case GREATER_OP :
        {
            fprintf(ASM_FILE, "ja ");
            break;
        }
        case LESS_EQUAL_OP :
        {
            fprintf(ASM_FILE, "jbe ");
            break;
        }
        case GREATER_EQUAL_OP :
        {
            fprintf(ASM_FILE, "jae ");
            break;
        }
    }

    size_t label = LABEL++;

    fprintf(ASM_FILE, "TRUE_%llu        \n"
                      "push 0           \n"
                      "jmp CONTINUE_%llu\n"
                      "TRUE_%llu :      \n"
                      "push 1           \n"
                      "jmp CONTINUE_%llu\n"
                      "CONTINUE_%llu :  \n", label, label, label, label, label);
}

size_t GetVarOfs(Function* function, char* name)
{
    assert(function);
    assert(name);

    for (size_t i = 0; i < function->num_args + function->num_vars; ++i)
    {
        if (strcmp(function->vars[i], name) == 0)
        {
            return i + 1;
        }
    }
    
    printf("error : '%s' wasn't declarated here\n", name);
    return -1;
}

NameTable* MakeTableOfNames(Tree* tree)
{
    assert(tree);

    size_t num_func = GetNumFunc(tree);

    NameTable* table = NewNameTable();
    table->functions = (Function*)calloc(num_func, sizeof(Function));
    table->num_func  = num_func;

    FillNameTable(table, tree);

    return table;
}

void DestructNameTable(NameTable* table)
{
    assert(table);

    for (size_t i = 0; i < table->num_func; ++i)
    {
        free(table->functions[i].vars);
    }
    
    free(table->functions);
}

void DeleteNameTable(NameTable* table)
{
    assert(table);

    free(table);
}

size_t GetNumFunc(Tree* tree)
{
    assert(tree);

    size_t result = 0;
    Node* node = tree->root;

    while (node != nullptr)
    {
        node = node->left;
        result++;
    }

    return result;
}

NameTable* NewNameTable()
{
    NameTable* table = (NameTable*)calloc(1, sizeof(NameTable));
    assert(table);

    table->num_func  = 0;
    table->functions = nullptr;

    return table;
}

void FillNameTable(NameTable* table, Tree* tree)
{
    assert(table);
    assert(tree);

    Node* node = tree->root;

    for (size_t i = 0; i < table->num_func; ++i)
    {
        GetFunction(node->right, table->functions + i);
        node = node->left;
    }
}

void GetFunction(Node* subtree, Function* function)
{
    assert(subtree);
    assert(function);

    function->name = subtree->value.name;

    function->num_args = GetNumArgs(subtree->right);
    function->num_vars = GetNumVars(subtree->left->right);

    function->vars = (char**)calloc(function->num_args + function->num_vars, sizeof(char*));

    GetNames(function->vars, subtree);
}

size_t GetNumArgs(Node* node)
{
    size_t result = 0;

    while (node != nullptr)
    {
        node = node->right;
        result++;
    }

    return result;
}

size_t GetNumVars(Node* node)
{
    if (node == nullptr) 
    {
        return 0;
    }

    if (node->type == DECL_TYPE)
    {
        return 1 + GetNumVars(node->left) + GetNumVars(node->right);
    }

    return GetNumVars(node->left) + GetNumVars(node->right);
}

void GetNames(char** vars, Node* subtree)
{
    assert(vars);
    assert(subtree);

    Node* arg = subtree->right;

    size_t ofs = 0;

    for (; arg != nullptr; ++ofs)
    {
        vars[ofs] = arg->value.name;

        arg = arg->right;
    }

    GetNewVars(subtree->left->right, vars, &ofs);
}

void GetNewVars(Node* node, char** vars, size_t* ofs)
{
    assert(vars);
    assert(ofs);

    if (node == nullptr)
    {
        return;
    }

    GetNewVars(node->left, vars, ofs);

    if (node->type == DECL_TYPE)
    {
        vars[*ofs] = node->left->value.name;
        (*ofs)++;
        return;
    }

    GetNewVars(node->right, vars, ofs);
}

void DumpNameTable(NameTable* table)
{
    assert(table);

    for (size_t i = 0; i < table->num_func; ++i)
    {
        printf("\n%s :\n", table->functions[i].name);
        printf("params : ");

        for (size_t j = 0; j < table->functions[i].num_args; ++j)
        {
            printf("%s, ", table->functions[i].vars[j]);
        }
        printf("\nvars : ");

        for (size_t j = table->functions[i].num_args; j < table->functions[i].num_args + 
                                                          table->functions[i].num_vars; ++j)
        {
            printf("%s, ", table->functions[i].vars[j]);
        }
    }
}
