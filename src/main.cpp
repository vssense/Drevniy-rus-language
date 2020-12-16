#include "compiling\\compiling.h"

// #include "C:\Users\vssen\Desktop\TX\TXLib.h"

const char* FILE_TO_WRITE = "tree.txt";

void Translate(const char* file);

int main(const int argc, const char* argv[])
{
    Parser* parser = Parse(argv[1]);
    // ParserDump(parser);

    Tree* tree = GetTree(parser);

    // WriteToFile(tree, FILE_TO_WRITE);

    // Translate(argv[1]);
        
    // TreeDump(tree);
    Assemble(tree);
    // TreeDump(new_tree);

    return 0;
}