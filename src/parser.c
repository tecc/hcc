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

void beginExpression(HcParser* parser) {

}

void beginStatement(HcParser* parser) {
    HcStatement statement;
    statement.tokenCount = 0;
    statement.tokens = malloc(1); 

    _Bool ended = false;

    _Bool expectKeyword = false;
    HcKeyword keywordToExpect = HC_KEYWORD_UNKNOWN;
    _Bool expectIdentifier = false;
    for (; !ended; parser->ci++) {
        HcToken* tokCurr = parser->tokens[parser->ci];


        // identifiers

        if (tokCurr->type == HC_TOKEN_TYPE_IDENTIFIER) {
            HcVarDeclarationStatement* varDecl;

            switch (statement.type) {
            case HC_STATEMENT_TYPE_VAR_DECLARATION:
                varDecl = statement.meta;
                if (varDecl->identifier.size != 0) {
                    varDecl->identifier = tokCurr->content;
                }
            }
        }
        
        if (tokCurr->type == HC_TOKEN_TYPE_KEYWORD) {
            HcKeyword keyword = HcKeyword_getKeyword(tokCurr->content);
            switch (keyword) {
            case HC_KEYWORD_LOCAL:
            case HC_KEYWORD_GLOBAL: 
            case HC_KEYWORD_SAVE:
                keywordToExpect = HC_KEYWORD_VAR;
                expectKeyword = true;
                break;
            case HC_KEYWORD_VAR:
                statement.type = HC_STATEMENT_TYPE_VAR_DECLARATION;
                HcVarDeclarationStatement* varDecl = malloc(sizeof(HcVarDeclarationStatement));
                if (parser->ci != 0) {
                    HcToken* prevToken = parser->tokens[parser->ci - 1];
                    HcKeyword kw = HcKeyword_getKeyword(prevToken->content);
                    switch (kw) {
                    case HC_KEYWORD_NATIVE:
                        varDecl->native = true;
                        varDecl->save = false;
                    case HC_KEYWORD_SAVE:
                        varDecl->save = true;
                    case HC_KEYWORD_GLOBAL:
                        varDecl->global = true;
                        break;
                    }
                }
                statement.meta = varDecl;
                
                break;
            }
        }
        if (expectKeyword) {
            // TODO: error system for parser - here is "Expected keyword, got <type>"
            
        }
    }
}

void HcParser_parse(HcParser* parser) {

}

