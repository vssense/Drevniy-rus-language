#include "parser.h"

const char* SKIP_SYMBOLS = " \n\t\r";

size_t LenOfFile(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t pos = ftell(file);
    fseek(file, 0, SEEK_SET);

    return pos;
}

Buffer* ReadFile(FILE* file)
{
    assert(file);

    Buffer* buffer = (Buffer*)calloc(1, sizeof(Buffer)); 
 
    size_t size = LenOfFile(file);

    buffer->str = (char*)calloc(size + 1, sizeof(char));
    assert(buffer->str);

    buffer->lenght = fread(buffer->str, sizeof(char), size, file);
    buffer->original_str = buffer->str;

    buffer->str[buffer->lenght] = '\0';

    return buffer;
}

Buffer* GetBuffer(const char* input)
{
    assert(input);

    FILE* file = fopen(input, "r");
    if (!file) 
    {
        printf("Error : no such file \"%s\"\n", input);
        assert(!"no input file\n");
    }

    return ReadFile(file); 
}

Token* NewTokensBuffer()
{
    Token* tokens = (Token*)calloc(MAX_SIZE, sizeof(Token));
    assert(tokens);

    return tokens;
}

void PrintError(ERROR error)
{
    switch (error)
    {
        case BUFFER_IS_OK :
        {
            printf("error was called, but not detected\n");
            break;
        }
        case TYPE_ERROR :
        {
            printf("unknown type\n");
            break;
        }
        case UNKNOWN_SYMBOL :
        {
            printf("unknown symbol\n");
            break;
        }
        case MISSED_BRACE :
        {
            printf("missed brace\n");
            break;
        }
        case MISSED_SMCLN :
        {
            printf("missed semicolon\n");
            break;   
        }
        case ASSG_ERROR :
        {
            printf("assignment error\n");
            break;   
        }
        default :
        {
            printf("unknown error\n");
            break;
        }
    }
}

void PrintBeforeSymbol(char* str, char symbol)
{
    int i = 0;
    while (str[i] != symbol && str[i] != '\0' && str[i] != EOF)
    {
        putchar(str[i++]);
    }
}

void CompilationError(Parser* parser, size_t* idx)
{
    assert(parser);

    printf("Compilation error : ");

    PrintError(parser->status);

    char* str_start = parser->tokens[*idx].token_str;
    
    while (str_start > parser->original_buffer && *str_start != '\n')
    {
        --str_start;
    }

    PrintBeforeSymbol(str_start, '\n');

    printf("\n");

    for (int i = 0; i <= parser->tokens[*idx].token_str - str_start; ++i)
    {
        printf(" ");
    }

    printf("^\n");
}

void IgnoreSpaces(Buffer* buffer)
{
    assert(buffer);

    buffer->str += strspn(buffer->str, SKIP_SYMBOLS);
}

#define SYMBOL *buffer->str
#define TYPE  tokens[*ofs].type
#define VALUE tokens[*ofs].value
#define STR   tokens[*ofs].token_str

void GetNumber(Token* tokens, size_t* ofs, Buffer* buffer)
{
    int len = 0;

    sscanf(buffer->str, "%lf%n", &VALUE.number, &len);
    
    TYPE = TYPE_NUMB;
    STR = buffer->str;
    (*ofs)++;

    buffer->str += len;
}

void GetName(Token* tokens, size_t* ofs, Buffer* buffer)
{
    for (int i = 0; i < NUM_OPERATORS; ++i)
    {
        if (strncmp(buffer->str, OPERATORS[i].name, OPERATORS[i].len) == 0)
        {
            VALUE.op = i;
            TYPE = TYPE_OP;
            STR = buffer->str;

            buffer->str += OPERATORS[i].len;
            (*ofs)++;
            return;
        }
    }

    int len = 0;

    VALUE.name = (char*)calloc(MAX_NAME_LEN, sizeof(char));
    sscanf(buffer->str, "%[A-Za-z0-9_]%lln", VALUE.name, &len);

    TYPE = TYPE_ID;
    STR = buffer->str;
    buffer->str += len;

    (*ofs)++;
}

void GetTokens(Token* tokens, size_t* ofs, Buffer* buffer)
{
    assert(tokens);
    assert(buffer);

    while (size_t(buffer->str - buffer->original_str) < buffer->lenght && SYMBOL != EOF)
    {
        IgnoreSpaces(buffer);

        if (isdigit(SYMBOL))
        {
            GetNumber(tokens, ofs, buffer);
        }
        else
        {
            GetName(tokens, ofs, buffer);
        }

        IgnoreSpaces(buffer);
    }
}

size_t FillTokens(Token* tokens, Buffer* buffer)
{
    assert(tokens);
    assert(buffer);

    size_t ofs = 0;

    GetTokens(tokens, &ofs, buffer);

    return ofs;
}

void Delete(Buffer* buffer)
{
    assert(buffer);

    free(buffer);
}

Parser* Parse(const char* input)
{
    assert(input);

    Buffer* buffer = GetBuffer(input);

    Token* tokens = NewTokensBuffer();

    size_t ofs = FillTokens(tokens, buffer);


    Parser* parser = (Parser*)calloc(1, sizeof(Parser));

    parser->tokens = tokens;
    parser->size = ofs;
    parser->original_buffer = buffer->original_str;

    Delete(buffer);

    return parser;
}

void ParserDump(Parser* parser)
{
    assert(parser);

    for (size_t i = 0; i < parser->size; ++i)
    {
        switch (parser->tokens[i].type)
        {
            case TYPE_NUMB :
            {
                printf("type number,   value = %lf\n", parser->tokens[i].value.number);
                break;
            }
            case TYPE_ID :
            {
                printf("type ID,       value = %s\n", parser->tokens[i].value.name);
                break;
            }
            case TYPE_OP :
            {
                printf("type operator, value = %s\n", OPERATORS[parser->tokens[i].value.op].name);
                break;
            }
            default :
            {
                printf("Unknown type, line = %d\n", __LINE__);
            }
        }
    }
}
