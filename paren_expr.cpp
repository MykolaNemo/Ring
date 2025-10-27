#include "paren_expr.h"
#include <iostream>

using namespace clang;

PAREN_EXPR::PAREN_EXPR():
    EXPR_NODE(NK_ParenExpr)
{
}

bool PAREN_EXPR::classof(const NODE *N)
{
    return N->getKind() == NK_ParenExpr;
}

std::variant<int, bool> PAREN_EXPR::execute()
{
    if(!children.empty())
    {
        return children.at(0)->execute();
    }
    return std::variant<int, bool>();
}
