#include "compiling.h"

size_t     GetNumFunc   (Tree* tree);
NameTable* NewNameTable ();
void       FillNameTable(NameTable* table, Tree* tree);
void       GetFunction  (Node* subtree, Function* function);
size_t     GetNumArgs   (Node* node);
size_t     GetNumVars   (Node* node);
void       GetNames     (char** vars, Node* subtree);
void       GetNewVars   (Node* node, char** vars, size_t* ofs);


NameTable* MakeTableOfNames(Tree* tree)
{
    assert(tree);

    size_t num_func = GetNumFunc(tree);

    NameTable* table = NewNameTable();
    table->functions = (Function*)calloc(num_func, sizeof(Function));
    table->num_func = num_func;

    FillNameTable(table, tree);

    return table;
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

    table->num_func = 0;
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
        vars[*ofs] = node->value.name;
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