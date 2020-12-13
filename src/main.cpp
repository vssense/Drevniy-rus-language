#include "compiling\\compiling.h"

// #include "C:\Users\vssen\Desktop\TX\TXLib.h"

const char* FILE_TO_WRITE = "tree.txt";

int main(const int argc, const char* argv[])
{
    Parser* parser = Parse(argv[1]);
 
    // ParserDump(parser);

    Tree* tree = GetTree(parser);

    WriteToFile(tree, FILE_TO_WRITE);

    Tree* new_tree = GetFromFile(FILE_TO_WRITE);
    

    TreeDump(tree);
    TreeDump(new_tree);

    return 0;
}