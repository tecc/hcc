#include <hcc/lexer.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STRING_WRAPPER_0 '"'
#define STRING_WRAPPER_1 '\''
#define ESCAPE_CHAR    '\\'
#define COMMENT_CHAR    '/'
#define COMMENT_SL_CHAR '/'
#define COMMENT_ML_CHAR '*'

HcString HcToken_toString(HcToken* token) {
    const HcChar* fmt = "UNKNOWN %s";
    switch (token->type) {
        case HC_TOKEN_TYPE_STRING:
            fmt = "STRING \"%s\"";
            break;
        case HC_TOKEN_TYPE_COMMENT_SL:
            fmt = "COMMENT";
            break;
        case HC_TOKEN_TYPE_COMMENT_ML:
            fmt = "MULTILINE COMMENT";
            break;
        case HC_TOKEN_TYPE_IDENTIFIER:
            fmt = "IDENTIFIER %s";
            break;
        case HC_TOKEN_TYPE_NUMBER:
            fmt = "NUMBER %s";
            break;
        case HC_TOKEN_TYPE_KEYWORD:
            fmt = "KEYWORD %s";
            break;
        case HC_TOKEN_TYPE_OPERATOR:
            fmt = "OPERATOR %s";
            break;
        default:
            fmt = "UNKNOWN %s";
    }

    HcChar* output = calloc(strlen(fmt) + token->content.size + 3, sizeof(HcChar));
    sprintf(output, fmt, HcString_toCharPtr(token->content));

    HcString result = HcString_new_a("{");
    result = HcString_append_s(result, output);
    result = HcString_append_c(result, '}');
    return result;
}

// keywords

HcKeyword HcKeyword_getKeyword(HcString string) {
    #define kw(w) if (HcString_compare(string, HcKeyword_getKeywordString(HC_KEYWORD_##w)) == 0) return HC_KEYWORD_##w
    kw(ON);
    kw(PROCESS);
    kw(FUNCTION);
    kw(VAR);
    kw(LOCAL);
    kw(GLOBAL);
    kw(SAVE);
    kw(NAMESPACE);
    kw(NATIVE);
    return HC_KEYWORD_UNKNOWN;
    #undef kw
}

HcString HcKeyword_getKeywordString(HcKeyword keyword) {
    switch (keyword) {
        case HC_KEYWORD_ON:
            return HcString_new("on", 2);
        case HC_KEYWORD_PROCESS:
            return HcString_new("process", 7);
        case HC_KEYWORD_FUNCTION:
            return HcString_new("function", 8);
        case HC_KEYWORD_VAR:
            return HcString_new("var", 3);
        case HC_KEYWORD_LOCAL:
            return HcString_new("local", 5);
        case HC_KEYWORD_GLOBAL:
            return HcString_new("global", 6);
        case HC_KEYWORD_SAVE:
            return HcString_new("save", 4);
        case HC_KEYWORD_NAMESPACE:
            return HcString_new("namespace", 9);
        case HC_KEYWORD_NATIVE:
            return HcString_new("native", 6);
        case HC_KEYWORD_UNKNOWN:
            return HcString_new(NULL, 0);
    }
}

// operators

HcOperator HcOperator_getOperator(HcString str) {
    #define op(w) if (HcString_compare(str, HcOperator_getOperatorString(HC_OPERATOR_##w))) return HC_OPERATOR_##w
    // misc
    op(BLOCK_BEGIN);
    op(BLOCK_END);
    op(LIST_BEGIN);
    op(LIST_END);
    op(PARAM_LIST_BEGIN);
    op(PARAM_LIST_END);
    op(STATEMENT_END);
    // arithmetic
    op(ADD);
    op(INCREMENT);
    op(INCREMENT_BY);
    op(SUBTRACT);
    op(DECREMENT);
    op(DECREMENT_BY);
    op(MULTIPLY);
    op(MULTIPLY_BY);
    op(DIVIDE);
    op(DIVIDE_BY);
    // comparison
    op(EQUAL);
    op(NOT_EQUAL);
    op(GREATER_THAN); op(GREATER_THAN_OR_EQUAL);
    op(LESS_THAN); op(LESS_THAN_OR_EQUAL);
    // boolean
    op(BOOL_OR); op(BOOL_AND);

    #undef op
    return HC_OPERATOR_UNKNOWN;
}

HcString HcOperator_getOperatorString(HcOperator op) {
    switch (op) {
        case HC_OPERATOR_ADD:
            return HcString_new("+", 1);
        case HC_OPERATOR_INCREMENT:
            return HcString_new("++", 2);
        case HC_OPERATOR_INCREMENT_BY:
            return HcString_new("+=", 2);
        case HC_OPERATOR_SUBTRACT:
            return HcString_new("-", 1);
        case HC_OPERATOR_DECREMENT:
            return HcString_new("--", 2);
        case HC_OPERATOR_DECREMENT_BY:
            return HcString_new("-=", 2);
        case HC_OPERATOR_MULTIPLY:
            return HcString_new("*", 1);
        case HC_OPERATOR_MULTIPLY_BY:
            return HcString_new("*=", 2);
        case HC_OPERATOR_DIVIDE:
            return HcString_new("/", 1);
        case HC_OPERATOR_DIVIDE_BY:
            return HcString_new("/=", 2);
        // misc
        case HC_OPERATOR_BLOCK_BEGIN:
            return HcString_new("{", 1);
        case HC_OPERATOR_BLOCK_END:
            return HcString_new("}", 1);
        case HC_OPERATOR_LIST_BEGIN:
            return HcString_new("[", 1);
        case HC_OPERATOR_LIST_END:
            return HcString_new("]", 1);
        case HC_OPERATOR_PARAM_LIST_BEGIN:
            return HcString_new("(", 1);
        case HC_OPERATOR_PARAM_LIST_END:
            return HcString_new(")", 1);
        case HC_OPERATOR_LIST_SEPARATOR:
            return HcString_new(",", 1);
        case HC_OPERATOR_MEMBER_ACCESSOR:
            return HcString_new(".", 1);
        case HC_OPERATOR_STATEMENT_END:
            return HcString_new("\n", 1);
        // comparison
        case HC_OPERATOR_EQUAL:
            return HcString_new("==", 2);
        case HC_OPERATOR_NOT_EQUAL:
            return HcString_new("!=", 2);
        case HC_OPERATOR_LESS_THAN:
            return HcString_new("<", 1);
        case HC_OPERATOR_LESS_THAN_OR_EQUAL:
            return HcString_new("<=", 2);
        case HC_OPERATOR_GREATER_THAN:
            return HcString_new(">", 1);
        case HC_OPERATOR_GREATER_THAN_OR_EQUAL:
            return HcString_new(">=", 2);
        case HC_OPERATOR_BOOL_OR:
            return HcString_new("||", 2);
        case HC_OPERATOR_BOOL_AND:
            return HcString_new("&&", 2);
        case HC_OPERATOR_UNKNOWN:
            return HcString_new(NULL, 0);
    }
}

// lexer

HcLexer HcLexer_new() {
    HcLexer result;
    result.content = HcString_new(NULL, 0);
    result.tokenCount = 0;
    result.tokens = malloc(1);
    result.errorCount = 0;
    result.errors = malloc(1);
    result.ci = 0;
    return result;
}

void HcLexer_setContent(HcLexer* lexer, HcString string) {
    lexer->content = string;
}

// Begin lexer code
HcToken* addToken(HcLexer* lexer, size_t offset) {
    if (lexer->tokenCount > 0) {
        HcToken* prev = lexer->tokens[lexer->tokenCount - 1];
        if (prev->type == HC_TOKEN_TYPE_UNKNOWN && prev->content.size == 0) {
            prev->offset = offset;
            return prev;
        }
    }
    // create token
    HcToken* tok = malloc(sizeof(HcToken));
    tok->content = HcString_new("", 0);
    tok->type = HC_TOKEN_TYPE_UNKNOWN;
    tok->offset = offset;

    // add token to lexer
    lexer->tokenCount++;
    lexer->tokens = realloc(lexer->tokens, sizeof(HcToken*) * lexer->tokenCount);
    lexer->tokens[lexer->tokenCount - 1] = tok;

    // return token
    return tok;
}

void addError(HcLexer* lexer, size_t offset, size_t length, HcString msg) {
    // create error
    HcLexerError err;
    err.offset = offset;
    err.length = length;
    err.msg = msg;

    // add error to lexer
    lexer->errorCount++;
    lexer->errors = realloc(lexer->errors, sizeof(HcLexerError) * lexer->errorCount);
    lexer->errors[lexer->errorCount - 1] = err;
}


bool isEscaped(HcString str, size_t index) {
    if (index == 0) {
        return false;
    }
    index--;
    if (HcString_getChar(str, index) == ESCAPE_CHAR) {
        return !isEscaped(str, index);
    }

    return false;
}

bool isFinishedOperator(HcOperator op) {
    switch (op) {
        // misc
        case HC_OPERATOR_BLOCK_BEGIN:
        case HC_OPERATOR_BLOCK_END:
        case HC_OPERATOR_LIST_BEGIN:
        case HC_OPERATOR_LIST_END:
        case HC_OPERATOR_PARAM_LIST_BEGIN:
        case HC_OPERATOR_PARAM_LIST_END:
        case HC_OPERATOR_LIST_SEPARATOR:
        case HC_OPERATOR_MEMBER_ACCESSOR:
        // arithmetic
        case HC_OPERATOR_INCREMENT:
        case HC_OPERATOR_INCREMENT_BY:
        case HC_OPERATOR_DECREMENT:
        case HC_OPERATOR_DECREMENT_BY:
        case HC_OPERATOR_MULTIPLY_BY:
        case HC_OPERATOR_DIVIDE_BY:
        // comparatison
        case HC_OPERATOR_EQUAL:
        case HC_OPERATOR_NOT_EQUAL:
        case HC_OPERATOR_LESS_THAN_OR_EQUAL:
        case HC_OPERATOR_GREATER_THAN_OR_EQUAL:
        case HC_OPERATOR_BOOL_OR:
        case HC_OPERATOR_BOOL_AND:
            return true;

        default:
            return false;
    }
}

bool isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

HcString* OPERATOR_CHARS = NULL;

bool isOperatorChar(char c) {
    return HcString_contains_c(*OPERATOR_CHARS, c);
}

bool isValidIdentifier(HcString s) {
    for (size_t i = 0; i < s.size; i++) {
        char c = HcString_getChar(s, i);
        if (HcString_contains_c(*OPERATOR_CHARS, c)) return false;
        // more chars here
    }
    return true;
}

#define sinit(var, data) if (var == NULL) { HcString temp = HcString_new_a(data); OPERATOR_CHARS = &temp; }

void HcLexer_lex(HcLexer* lexer) {
    sinit(OPERATOR_CHARS, "+-*/:={}[]()^|&?%,.@");
    HcToken* token = addToken(lexer, 0);
    lexer->tokenCount--;

    // errors

    // lex loop
    // very complex
    for (lexer->ci = 0; lexer->ci < lexer->content.size; lexer->ci++) {
        HcChar c = HcString_getChar(lexer->content, lexer->ci);
        // string appending & ending
        if (token->type == HC_TOKEN_TYPE_STRING) {
            // if char is string end
            // bit complex as it has to match the beginning
            // but thats easy to check
            if (c == HcString_getChar(lexer->content, lexer->ci - token->content.size - 1)) {
                // if char is not escaped
                if (!isEscaped(token->content, token->content.size)) {
                    // end string
                    token = addToken(lexer, lexer->ci + 1);
                    continue;
                }
            }
            token->content = HcString_append_c(token->content, c);
            continue;
        }
        // string begin
        if (c == STRING_WRAPPER_0 || c == STRING_WRAPPER_1) {
            token = addToken(lexer, lexer->ci);
            token->type = HC_TOKEN_TYPE_STRING;
            continue;
        }
        // comment appending & ending
        if (token->type == HC_TOKEN_TYPE_COMMENT_SL || token->type == HC_TOKEN_TYPE_COMMENT_ML) {
            if (token->type == HC_TOKEN_TYPE_COMMENT_SL && c == '\n') {
                token = addToken(lexer, lexer->ci + 1);
                continue;
            }
            if (token->type == HC_TOKEN_TYPE_COMMENT_ML) {
                if (c == COMMENT_CHAR && HcString_getChar(lexer->content, lexer->ci - 1) == COMMENT_ML_CHAR) {
                    token = addToken(lexer, lexer->ci + 1);
                    continue;
                }
            }
            token->content = HcString_append_c(token->content, c);
            continue;
        }
        // comment begin & end
        if (lexer->ci > 0 && (c == COMMENT_SL_CHAR || c == COMMENT_ML_CHAR)) {
            if (HcString_getChar(lexer->content, lexer->ci - 1) == COMMENT_CHAR) {
                token->type = c == COMMENT_SL_CHAR ? HC_TOKEN_TYPE_COMMENT_SL : HC_TOKEN_TYPE_COMMENT_ML;
                continue;
            }
        }

        // whitespace check
        if (isWhitespace(c)) {
            if (HcKeyword_getKeyword(token->content) != HC_KEYWORD_UNKNOWN) {
                token->type = HC_TOKEN_TYPE_KEYWORD;
            }
            token = addToken(lexer, lexer->ci + 1);
            continue;
        }
        // operator

        if (token->type == HC_TOKEN_TYPE_OPERATOR) {
            if (!isOperatorChar(c)) {
                token = addToken(lexer, lexer->ci);
                token->content = HcString_append_c(token->content, c);
                continue;
            } else {
                if (isFinishedOperator(HcOperator_getOperator(token->content))) {
                    goto operator_begin;
                }
                token->content = HcString_append_c(token->content, c);
            }
        }

        if (isOperatorChar(c)) {
            operator_begin:
            token = addToken(lexer, lexer->ci);
            token->type = HC_TOKEN_TYPE_OPERATOR;
            token->content = HcString_append_c(token->content, c);
            continue;
        }

        token->content = HcString_append_c(token->content, c);
    }
    // second pass for stuff
    for (size_t i = 0; i < lexer->tokenCount; i++) {
        token = lexer->tokens[i];
        if (token->type == HC_TOKEN_TYPE_UNKNOWN) {
            if (isValidIdentifier(token->content)) {
                token->type = HC_TOKEN_TYPE_IDENTIFIER;
                continue;
            }
        }
    }
}
