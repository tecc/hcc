#include <hcc/lexer.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

HcString HcToken_toString(HcToken* token) {
    const HcChar* fmt;
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
    #define op(w) if (HcString_compare(str, HcOperator_getOperatorString(HC_OPERATOR_##w)) == 0) return HC_OPERATOR_##w
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
        default:
            return HcString_new(NULL, 0);
    }
}