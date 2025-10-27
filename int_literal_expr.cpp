#include "int_literal_expr.h"

INT_LITERAL_EXPR::INT_LITERAL_EXPR(int _v):
    EXPR_NODE(NK_IntLiteralExpr),
    v(_v)
{
}

bool INT_LITERAL_EXPR::classof(const NODE *N)
{
    return N->getKind() == NK_IntLiteralExpr;
}

std::variant<int, bool> INT_LITERAL_EXPR::execute()
{
    return std::variant<int, bool>(v);
}
