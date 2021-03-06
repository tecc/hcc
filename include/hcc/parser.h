#ifndef HCC_PARSER_H
#define HCC_PARSER_H

#include <hcc/lexer.h>
#include <hcc/string.h>

// expressions
typedef struct HcExpression_ {} HcExpression;

// statements
typedef enum HcStatementType_ {
    HC_STATEMENT_TYPE_BLOCK,
    HC_STATEMENT_TYPE_DECLARATION,
    HC_STATEMENT_TYPE_VAR_DECLARATION, // distinct type because it makes my life easier
    HC_STATEMENT_TYPE_NAMESPACE_DECLARATION
} HcStatementType;

typedef struct HcStatement_ {
    HcStatementType type;
    void* meta;
    HcToken** tokens;
    size_t tokenCount;
} HcStatement;

// block
typedef struct HcBlockStatement_ {
    HcStatement** children;
    size_t statements;
} HcBlockStatement;

// declaration statement

typedef enum HcDeclarationType_ {
    HC_DECLARATION_TYPE_ON,
    HC_DECLARATION_TYPE_FUNCTION,
    HC_DECLARATION_TYPE_PROCESS,
} HcDeclarationType;

typedef struct HcParameter_ {
    HcString identifier;
    HcString type;
} HcParameter;

typedef struct HcDeclarationStatement_ {
    HcDeclarationType type;
    HcString identifier;
    HcParameter* data;
} HcDeclarationStatement;

// var declaration statement
typedef struct HcVarDeclarationStatement_ {
    HcString identifier;
    HcString* type;
    _Bool save;
    _Bool global;
    _Bool native;
} HcVarDeclarationStatement;
// namespace declaration statement
typedef struct HcNamespaceDeclarationStatement_ {
    HcString identifier;
    HcBlockStatement* block;
} HcNamespaceDeclarationStatement;

typedef struct HcParserError_ {
    size_t token;
    HcString message;
} HcParserError;

// parser functions
typedef struct HcParser_ {
    HcToken** tokens;
    size_t tokenCount;
    HcStatement** statements;
    size_t statementCount;
    HcParserError* errors;
    size_t errorCount;
    size_t ci;
} HcParser;

HcString HcStatementType_toString(HcStatementType type);
HcString HcStatement_detailsToString(HcStatement* statement);
HcString HcStatement_toString(HcStatement* statement);

HcParser HcParser_new();
void HcParser_setTokens(HcParser* parser, HcToken** tokens, size_t count);
void HcParser_parse(HcParser* parser);



#endif
