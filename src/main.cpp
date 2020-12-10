#include "tree.h"

int main(const int argc, const char* argv[])
{
    Parser* parser = Parse(argv[1]);

    ParserDump(parser);

    Tree* tree = GetTree(parser);
    
    TreeDump(tree);

    return 0;
}