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

void IMPLICIT_CAST_EXPR::addChild(NODE *child)
{
    children.push_back(child);
    if(isa<DECL_REF_EXPR>(child))
    {
        decl = dyn_cast<DECL_REF_EXPR>(child);
    }
}

int IMPLICIT_CAST_EXPR::execute()
{
    if(decl)
    {
        return decl->execute();
    }
    return 0;
}
