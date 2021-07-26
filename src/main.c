#include <stdio.h>
#include <stdlib.h>
#include <hcc/string.h>
#include <hcc/lexer.h>
#include <hcc/parser.h>

#define MAX_LENGTH 20000

int main(int argc, char** argv) {
    printf("Hypercode Compiler (hcc)\n");

    if (argc < 2) {
        printf("Not enough arguments.\n");
        return 1;
    }
    char* file = argv[1];
    FILE* fp = fopen(file, "r");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* data = calloc(size, sizeof(char));
    for (size_t i = 0; i < size; i++) {
        data[i] = (char) fgetc(fp);
    }

    HcString str = HcString_new(data, size);

    // lexing
    HcLexer lexer = HcLexer_new();
    HcLexer_setContent(&lexer, str);
    HcLexer_lex(&lexer);
    // debug stuff
    #ifdef DEBUG
    printf("Printing tokens (%zu)...\n", lexer.tokenCount);
    for (size_t i = 0; i < lexer.tokenCount; i++) {
        printf("Token %05zu: %s\n", i, HcString_toCharPtr(HcToken_toString(lexer.tokens[i])));
    }
    #endif

    // parsing
    HcParser parser = HcParser_new();
    HcParser_setTokens(&parser, lexer.tokens, lexer.tokenCount);
    HcParser_parse(&parser);
    #ifdef DEBUG
    printf("Printing statements (%zu)...\n", parser.statementCount);
    for (size_t i = 0; i < parser.statementCount; i++) {
        printf("Statement %05zu: %s\n", i, HcString_toCharPtr(HcStatement_toString(parser.statements[i])));
    }
    #endif

    return 0;
}
