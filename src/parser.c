#include <hcc/parser.h>
#include <stdlib.h>

HcParser HcParser_new() {
    HcParser parser;
    parser.tokens = NULL;
    parser.tokenCount = 0;
    parser.statements = malloc(1);
    parser.statementCount = 0;
    parser.errors = malloc(1);
    parser.errorCount = 0;
    parser.ci = 0;
    return parser;
}

void HcParser_setTokens(HcParser* parser, HcToken** tokens, size_t count) {
    parser->tokens = tokens;
    parser->tokenCount = count;
}


void beginExpression(HcParser* parser) {

}

void beginStatement(HcParser* parser);

void parseToBlock(HcParser* parser, HcBlockStatement* block, size_t count) {
    HcParser subparser = HcParser_new();
    HcParser_setTokens(&subparser, parser->tokens + parser->ci, count);
    HcParser_parse(&subparser);
    block->children = subparser.statements;
    block->statements = subparser.statementCount;
}

void addError(HcParser* parser, HcString msg) {
    HcParserError error;
    error.token = parser->ci;
    error.message = msg;

    parser->errorCount++;
    parser->errors = realloc(parser->errors, sizeof(HcParserError) * parser->errorCount);
    parser->errors[parser->errorCount - 1] = error;
}
void addError_a(HcParser* parser, const char* msg) {
    addError(parser, HcString_new_a(msg));
}

void beginStatement(HcParser* parser) {
    HcStatement statement;
    statement.tokenCount = 0;
    statement.tokens = malloc(1);

    _Bool ended = false;

    _Bool expectKeyword = false;
    HcKeyword keywordToExpect = HC_KEYWORD_UNKNOWN;
    _Bool expectIdentifier = false;
    _Bool expectBlock = false;
    for (; !ended && parser->ci < parser->tokenCount; parser->ci++) {
        HcToken* tokCurr = parser->tokens[parser->ci];
        if (tokCurr->type == HC_TOKEN_TYPE_COMMENT_SL ||
            tokCurr->type == HC_TOKEN_TYPE_COMMENT_ML) {
            continue;
        }

        // operator
        if (tokCurr->type == HC_TOKEN_TYPE_OPERATOR) {
            HcOperator operator = HcOperator_getOperator(tokCurr->content);
            HcBlockStatement* block = malloc(sizeof(HcBlockStatement));
            switch (operator) {
                case HC_OPERATOR_BLOCK_BEGIN:
                {
                    size_t blockEnd = parser->ci + 1, blockDepth = 1;
                    bool found = false;
                    for (!found; blockEnd < parser->tokenCount; blockEnd++) {
                        HcToken* token = parser->tokens[blockEnd];
                        if (token->type == HC_TOKEN_TYPE_OPERATOR) {
                            HcOperator op = HcOperator_getOperator(token->content);
                            if (op == HC_OPERATOR_BLOCK_BEGIN) {
                                blockDepth++;
                                continue;
                            }
                            if (op == HC_OPERATOR_BLOCK_END) {
                                blockDepth--;
                            }
                        }
                        if (blockDepth == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        parseToBlock(parser, block, blockEnd - parser->ci);
                    } else {
                        addError_a(parser, "End of block not found");
                    }
                }

            }
        }

        // identifiers

        if (tokCurr->type == HC_TOKEN_TYPE_IDENTIFIER) {
            HcVarDeclarationStatement* varDecl;
            HcNamespaceDeclarationStatement* nsDecl;

            switch (statement.type) {
                case HC_STATEMENT_TYPE_VAR_DECLARATION:
                    varDecl = statement.meta;
                    if (varDecl->identifier.size == 0) {
                        varDecl->identifier = tokCurr->content;
                    }
                    break;
                case HC_STATEMENT_TYPE_NAMESPACE_DECLARATION:
                    nsDecl = statement.meta;
                    if (nsDecl->identifier.size == 0) {
                        nsDecl->identifier = tokCurr->content;
                    }
                    expectIdentifier = false;
                    expectBlock = true;
                default:
                    break;
            }
        }

        if (tokCurr->type == HC_TOKEN_TYPE_KEYWORD) {
            HcKeyword keyword = HcKeyword_getKeyword(tokCurr->content);

            if (expectKeyword) {
                if (keyword != keywordToExpect && keywordToExpect != HC_KEYWORD_UNKNOWN) {
                    // TODO: error system for parser - here is "Expected keyword <x>, got <y>"
                } else {
                    keywordToExpect = HC_KEYWORD_UNKNOWN;
                }
            }

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
                        if (prevToken->type == HC_TOKEN_TYPE_KEYWORD) {
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
                    }

                    statement.meta = varDecl;
                    break;
                case HC_KEYWORD_NAMESPACE:
                    statement.type = HC_STATEMENT_TYPE_NAMESPACE_DECLARATION;
                    HcNamespaceDeclarationStatement* nsDecl = malloc(sizeof(HcNamespaceDeclarationStatement));
                    statement.meta = nsDecl;
                    expectIdentifier = true;
                    break;
            }
        }
        if (expectKeyword) {
            // TODO: error system for parser - here is "Expected keyword, got <type>"

        }
    }

    parser->statementCount++;
    parser->statements = reallocarray(parser->statements, parser->statementCount, sizeof(HcStatement));
    parser->statements[parser->statementCount - 1] = &statement;
}

void HcParser_parse(HcParser* parser) {
    while (parser->ci < parser->tokenCount) {
        beginStatement(parser);
    }
}

