#include "..//tree//tree.h"

static const char* SCAN  = "nepravdoi";
static const char* PRINT = "govoru";
static const char* SQRT  = "koren";
static const char* FLOOR = "okrugli";

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

struct Compilier
{
    NameTable* table;
    Function* function;

    FILE* file;

    size_t label;
};

NameTable* MakeTableOfNames(Tree* tree);
void       DumpNameTable   (NameTable* table);
void       Assemble        (Tree* tree);
void       Translate       (const char* file);
void       Compile         (const char* input);
