#include "decl_ref_expr.h"
#include "var_decl.h"
#include <iostream>

using namespace clang;

DECL_REF_EXPR::DECL_REF_EXPR():
    EXPR_NODE(NK_DeclRefExpr)
{

}

bool DECL_REF_EXPR::classof(const NODE *N)
{
    return N->getKind() == NK_DeclRefExpr;
}

void DECL_REF_EXPR::addChild(NODE *child)
{
    children.push_back(child);
    if(!decl && isa<VAR_DECL>(child))
    {
        decl = dyn_cast<VAR_DECL>(child);
    }
    else
    {
        std::cout<<"DECL_REF_EXPR can not have more than one child"<<std::endl;
    }
}

int DECL_REF_EXPR::execute()
{
    if(decl)
    {
        return decl->m_value;
    }
    return 0;
}

void DECL_REF_EXPR::setValue(int v)
{
    if(decl)
    {
        decl->m_value = v;
    }
}
