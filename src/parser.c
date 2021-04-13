#include <hcc/parser.h>
#include <stdlib.h>

HcParser HcParser_new() {
    HcParser parser;
    parser.tokens = NULL;
    parser.tokenCount = 0;
    parser.statements = malloc(1);
    parser.statementCount = 0;
    parser.ci = 0;
    return parser;
}

void HcParser_setTokens(HcParser* parser, HcToken** tokens, size_t count) {
    parser->tokens = tokens;
    parser->tokenCount = count;
}

void HcParser_parse(HcParser* parser) {

}

