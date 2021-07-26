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
HcToken* HcLexer_addToken(HcLexer* lexer, size_t offset) {
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

void HcLexer_addError(HcLexer* lexer, size_t offset, size_t length, HcString msg) {
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
        // comparison
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
    HcToken* token = HcLexer_addToken(lexer, 0);
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
                    token = HcLexer_addToken(lexer, lexer->ci + 1);
                    continue;
                }
            }
            token->content = HcString_append_c(token->content, c);
            continue;
        }
        // string begin
        if (c == STRING_WRAPPER_0 || c == STRING_WRAPPER_1) {
            token = HcLexer_addToken(lexer, lexer->ci);
            token->type = HC_TOKEN_TYPE_STRING;
            continue;
        }
        // comment appending & ending
        if (token->type == HC_TOKEN_TYPE_COMMENT_SL || token->type == HC_TOKEN_TYPE_COMMENT_ML) {
            if (token->type == HC_TOKEN_TYPE_COMMENT_SL && c == '\n') {
                token = HcLexer_addToken(lexer, lexer->ci + 1);
                continue;
            }
            if (token->type == HC_TOKEN_TYPE_COMMENT_ML) {
                if (c == COMMENT_CHAR && HcString_getChar(lexer->content, lexer->ci - 1) == COMMENT_ML_CHAR) {
                    token = HcLexer_addToken(lexer, lexer->ci + 1);
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
            token = HcLexer_addToken(lexer, lexer->ci + 1);
            continue;
        }
        // operator

        if (token->type == HC_TOKEN_TYPE_OPERATOR) {
            if (!isOperatorChar(c)) {
                token = HcLexer_addToken(lexer, lexer->ci);
                token->content = HcString_append_c(token->content, c);
                continue;
            } else {
                if (isFinishedOperator(HcOperator_getOperator(token->content))) {
                    goto operator_begin;
                }
                token->content = HcString_append_c(token->content, c);
                continue;
            }
        }

        if (isOperatorChar(c)) {
            operator_begin:
            token = HcLexer_addToken(lexer, lexer->ci);
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
