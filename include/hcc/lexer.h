#ifndef HCC_LEXER_H
#define HCC_LEXER_H

#include <hcc/string.h>

typedef enum HcTokenType_ {
    HC_TOKEN_TYPE_UNKNOWN,
    HC_TOKEN_TYPE_STRING,
    HC_TOKEN_TYPE_NUMBER,
    HC_TOKEN_TYPE_KEYWORD,
    HC_TOKEN_TYPE_OPERATOR,
    HC_TOKEN_TYPE_IDENTIFIER,
    HC_TOKEN_TYPE_COMMENT_SL,
    HC_TOKEN_TYPE_COMMENT_ML
} HcTokenType;

typedef enum HcKeyword_ {
    HC_KEYWORD_UNKNOWN,
    HC_KEYWORD_ON,
    HC_KEYWORD_PROCESS,
    HC_KEYWORD_FUNCTION,
    HC_KEYWORD_VAR,
    HC_KEYWORD_LOCAL,
    HC_KEYWORD_GLOBAL,
    HC_KEYWORD_SAVE,
    HC_KEYWORD_NAMESPACE,
    HC_KEYWORD_NATIVE,
} HcKeyword;

HcKeyword HcKeyword_getKeyword(HcString string);
HcString HcKeyword_getKeywordString(HcKeyword keyword);

typedef enum HcOperator_ {
    HC_OPERATOR_UNKNOWN,
    // misc
    HC_OPERATOR_BLOCK_BEGIN, HC_OPERATOR_BLOCK_END, // { and }
    HC_OPERATOR_LIST_BEGIN, HC_OPERATOR_LIST_END, // [ and ]
    HC_OPERATOR_PARAM_LIST_BEGIN, HC_OPERATOR_PARAM_LIST_END, // ( and )
    HC_OPERATOR_LIST_SEPARATOR, // ,
    HC_OPERATOR_MEMBER_ACCESSOR, // .
    HC_OPERATOR_STATEMENT_END, // newline \n
    // arithmetic
    HC_OPERATOR_ADD, HC_OPERATOR_INCREMENT, HC_OPERATOR_INCREMENT_BY, // +, ++, and +=
    HC_OPERATOR_SUBTRACT, HC_OPERATOR_DECREMENT, HC_OPERATOR_DECREMENT_BY, // -, --, and -=
    HC_OPERATOR_MULTIPLY, HC_OPERATOR_MULTIPLY_BY, // * and *=
    HC_OPERATOR_DIVIDE, HC_OPERATOR_DIVIDE_BY, // / and /=
    // comparison
    HC_OPERATOR_EQUAL, HC_OPERATOR_NOT_EQUAL, // == and !=
    HC_OPERATOR_LESS_THAN, HC_OPERATOR_LESS_THAN_OR_EQUAL, // < and <=
    HC_OPERATOR_GREATER_THAN, HC_OPERATOR_GREATER_THAN_OR_EQUAL, // > and >=
    // boolean
    HC_OPERATOR_BOOL_OR, HC_OPERATOR_BOOL_AND, // || and &&
} HcOperator;

HcOperator HcOperator_getOperator(HcString string);
HcString HcOperator_getOperatorString(HcOperator op);

typedef struct HcToken_ {
    HcTokenType type;
    HcString content;
    size_t offset;
} HcToken;

HcString HcToken_toString(HcToken* token);

typedef struct HcLexerError_ {
    size_t offset;
    size_t length;
    HcString msg;
} HcLexerError;

typedef struct HcLexer_ {
    HcToken** tokens;
    size_t tokenCount;
    HcLexerError* errors;
    size_t errorCount;
    HcString content;
    size_t ci;
} HcLexer;

HcLexer HcLexer_new();
void HcLexer_setContent(HcLexer* lexer, HcString content);
void HcLexer_lex(HcLexer* lexer);

#endif
