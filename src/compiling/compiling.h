#include "..//tree//tree.h"

struct Function
{
    const char* name;

    char** vars;

    size_t num_args;
    size_t num_vars;
};

struct NameTable
{
    Function* functions;

    size_t num_func;
};

NameTable* MakeTableOfNames(Tree* tree);
void       DumpNameTable   (NameTable* table);
