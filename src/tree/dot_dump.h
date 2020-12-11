const char* STANDARD_DOT_TXT_FILE_NAME = "log\\tree.txt";
const char*  DUMP_FILE_NAME = "log\\numsjpg.txt";

const size_t DOT_CMD_SIZE = 64;
const size_t JPG_CMD_SIZE = 32;
const size_t NUM_STR_LEN = 5;
const size_t NOTATION = 10;

size_t GetJPGNumber()
{
    FILE* numjpgs = fopen(DUMP_FILE_NAME, "r");

    size_t num = 0;
    
    if (numjpgs != nullptr)
    {
        fscanf(numjpgs, "%llu", &num);
    }

    fclose(numjpgs);

    numjpgs = fopen(DUMP_FILE_NAME, "w");
    fprintf(numjpgs, "%llu", num + 1);
    fclose(numjpgs);
    
    return num;
}

void GetNames(char* dot_cmd, char* jpg_cmd)
{
    assert(dot_cmd);
    assert(jpg_cmd);

    size_t num = GetJPGNumber();

    char num_str[NUM_STR_LEN] = "";
    itoa(num, num_str, NOTATION);
    char extension[] = ".svg";


    strcat(dot_cmd, num_str);
    strcat(dot_cmd, extension);
    
    strcat(jpg_cmd, num_str);
    strcat(jpg_cmd, extension);
}

void PrintNodes(Tree* tree, Node* node, FILE* dump_file)
{
    assert(tree);
    assert(dump_file);

    if (node->type == ID_TYPE || node->type == DECL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
                node, node->type, node->value.name);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
        //         node, node->type, node->value.name); 
    }
    else if (node->type == NUMB_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %lg\"]",
                node, node->type, node->value.number); 
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %lg\"]",
        //         node, node->type, node->value.number); 
    }
    else if (node->type == D_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EBF7F7\", label=\"%s | %d\"]",
                node, "fict", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#EBF7F7\", label=\"%s | %d\"]",
        //         node, "fict", node->value.op);
    }
    else if (node->type == CALL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#AEE8C5\", label=\"%s | %d\"]",
                node, "call", node->value.op);        
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#AEE8C5\", label=\"%s | %d\"]",
        //         node, "call", node->value.op);
    }
    else if (node->type == STAT_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#A2F39D\", label=\"%s | %d\"]",
                node, "S", node->value.op);           
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#A2F39D\", label=\"%s | %d\"]",
        //         node, "S", node->value.op);
    }
    else if (node->type == COND_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
                node, "if", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
        //         node, "if", node->value.op);
    } 
    else if (node->type == IFEL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
                node, "if-else", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
        //         node, "if-else", node->value.op);

    }
    else if (node->type == JUMP_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#BC8ED7\", label=\"%s | %d\"]",
                node, "return", node->value.op);           
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#BC8ED7\", label=\"%s | %d\"]",
        //         node, "return", node->value.op);
    }
    else if (node->type == LOOP_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#AEDEE8\", label=\"%s | %d\"]",
                node, "while", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#AEDEE8\", label=\"%s | %d\"]",
        //         node, "while", node->value.op);

    }
    else if (node->type == ARG_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F0F5F1\", label=\"%s | %d\"]",
               node, "arg", node->value.op);       
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F0F5F1\", label=\"%s | %d\"]",
        //        node, "arg", node->value.op);
    }
    else
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
               node, node->type, OPERATORS[node->value.op].name);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
        //        node, node->type, OPERATORS[node->value.op].name);
    }

    if (node->left != nullptr)
    {
        fprintf(dump_file, "\"%p\":sw->\"%p\";\n", node, node->left);
        PrintNodes(tree, node->left, dump_file);
    }
    
    if (node->right != nullptr)
    {
        fprintf(dump_file, "\"%p\":se->\"%p\";\n", node, node->right);
        PrintNodes(tree, node->right, dump_file);
    }
}

void PrintNodesHard(Tree* tree, Node* node, FILE* dump_file)
{
    assert(tree);
    assert(dump_file);

    if (node->type == ID_TYPE || node->type == DECL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s",
                node, node->type, node->value.name);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
        //         node, node->type, node->value.name); 
    }
    else if (node->type == NUMB_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %lg",
                node, node->type, node->value.number); 
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %lg\"]",
        //         node, node->type, node->value.number); 
    }
    else if (node->type == D_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#EBF7F7\", label=\"%s | %d",
                node, "fict", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#EBF7F7\", label=\"%s | %d\"]",
        //         node, "fict", node->value.op);
    }
    else if (node->type == CALL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#AEE8C5\", label=\"%s | %d",
                node, "call", node->value.op);        
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#AEE8C5\", label=\"%s | %d\"]",
        //         node, "call", node->value.op);
    }
    else if (node->type == STAT_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#A2F39D\", label=\"%s | %d",
                node, "S", node->value.op);           
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#A2F39D\", label=\"%s | %d\"]",
        //         node, "S", node->value.op);
    }
    else if (node->type == COND_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d",
                node, "if", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
        //         node, "if", node->value.op);
    } 
    else if (node->type == IFEL_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d",
                node, "if-else", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F3DF9D\", label=\"%s | %d\"]",
        //         node, "if-else", node->value.op);

    }
    else if (node->type == JUMP_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#BC8ED7\", label=\"%s | %d",
                node, "return", node->value.op);           
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#BC8ED7\", label=\"%s | %d\"]",
        //         node, "return", node->value.op);
    }
    else if (node->type == LOOP_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#AEDEE8\", label=\"%s | %d",
                node, "while", node->value.op);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#AEDEE8\", label=\"%s | %d\"]",
        //         node, "while", node->value.op);

    }
    else if (node->type == ARG_TYPE)
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#F0F5F1\", label=\"'%s' | %d",
               node, "arg", node->value.op);       
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#F0F5F1\", label=\"%s | %d\"]",
        //        node, "arg", node->value.op);
    }
    else
    {
        fprintf(dump_file, "\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s",
               node, node->type, OPERATORS[node->value.op].name);
        // printf("\"%p\"[style=\"filled\", fillcolor=\"#C0F5F1\", label=\"%d | %s\"]",
        //        node, node->type, OPERATORS[node->value.op].name);
    }

    fprintf(dump_file, " | {n : %p | %p}\"]", node, node->parent);

    if (node->left != nullptr)
    {
        fprintf(dump_file, "\"%p\":sw->\"%p\";\n", node, node->left);
        PrintNodesHard(tree, node->left, dump_file);
    }
    
    if (node->right != nullptr)
    {
        fprintf(dump_file, "\"%p\":se->\"%p\";\n", node, node->right);
        PrintNodesHard(tree, node->right, dump_file);
    }
}

void TreeDump(Tree* tree)
{
    assert(tree);

    FILE* dump_file = fopen(STANDARD_DOT_TXT_FILE_NAME, "w");

    fprintf(dump_file, "digraph G{\n");
    fprintf(dump_file, "node [shape=\"circle\"]\n");

    PrintNodes(tree, tree->root, dump_file);

    fprintf(dump_file, "}");

    fclose(dump_file);

    char dot_cmd[DOT_CMD_SIZE] = "";
    snprintf(dot_cmd, DOT_CMD_SIZE, "dot -Tsvg %s -o log\\Dump", STANDARD_DOT_TXT_FILE_NAME);
    char jpg_cmd[JPG_CMD_SIZE] = "start log\\Dump";
    
    GetNames(dot_cmd, jpg_cmd);

    system(dot_cmd);
    system(jpg_cmd);
}