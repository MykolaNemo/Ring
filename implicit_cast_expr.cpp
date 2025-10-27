#include "implicit_cast_expr.h"
#include "decl_ref_expr.h"
#include <iostream>

using namespace clang;

IMPLICIT_CAST_EXPR::IMPLICIT_CAST_EXPR():
    EXPR_NODE(NK_ImplicitCastExpr)
{

}

bool IMPLICIT_CAST_EXPR::classof(const NODE *N)
{
    return N->getKind() == NK_ImplicitCastExpr;
}

std::variant<int, bool> IMPLICIT_CAST_EXPR::execute()
{
    if(!children.empty())
    {
        return children.at(0)->execute();
    }
    return std::variant<int, bool>();
}
