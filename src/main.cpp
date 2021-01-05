#include "compiling\\compiling.h"

const char* FILE_TO_WRITE = "tree_in_written_form.txt";

void WriteTreeToFile(const char* file)
{
    assert(file);

    Parser* parser = Parse(file);
    Tree* tree = GetTree(parser);

    WriteToFile(tree, FILE_TO_WRITE);

    DestructTree(tree);
    DeleteTree(tree);

    DestructParser(parser);
    DeleteParser(parser);
}

int main(const int argc, const char* argv[])
{
    Compile(argv[1]);

    // WriteTreeToFile(argv[1]);

    // Translate(argv[1]);

    return 0;
}