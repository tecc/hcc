#include <hcc/parser.h>

HcString HcStatementType_toString(HcStatementType type) {
    switch (type) {
        case HC_STATEMENT_TYPE_BLOCK:
            return HcString_new_a("BLOCK");
        case HC_STATEMENT_TYPE_DECLARATION:
            return HcString_new_a("DECL");
        case HC_STATEMENT_TYPE_VAR_DECLARATION:
            return HcString_new_a("DECL_VAR");
        case HC_STATEMENT_TYPE_NAMESPACE_DECLARATION:
            return HcString_new_a("DECL_NS");
        default:
            return HcString_new_a("UNKNOWN");
    }
}

HcString HcStatement_detailsToString(HcStatement* statement) {
    switch (statement->type) {
        case HC_STATEMENT_TYPE_NAMESPACE_DECLARATION:
        {
            HcNamespaceDeclarationStatement* nsdecl = (HcNamespaceDeclarationStatement*) statement->meta;
            return nsdecl->identifier;
        }
        case HC_STATEMENT_TYPE_VAR_DECLARATION:
        {
            HcVarDeclarationStatement* vardecl = (HcVarDeclarationStatement*) statement->meta;
            return HcString_format("t:%s i:%s", 128, HcString_toCharPtr_p(vardecl->type), HcString_toCharPtr(vardecl->identifier));
        }
        default:
            return HcString_new(NULL, 0);
    }
}

HcString HcStatement_toString(HcStatement* statement) {
    HcString type = HcStatementType_toString(statement->type);
    HcString details = HcStatement_detailsToString(statement);

    HcString result = HcString_new("{", 1);
    result = HcString_append(result, type);
    if (details.size != 0) {
        result = HcString_append_c(result, ' ');
        result = HcString_append(result, details);
    }
    result = HcString_append_c(result, '}');
    return result;
}