#include "decl_ref_expr.h"
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
    if(!var_decl && isa<VAR_DECL>(child))
    {
        var_decl = dyn_cast<VAR_DECL>(child);
    }
    else
    {
        std::cout<<"DECL_REF_EXPR can not have more than one child"<<std::endl;
    }
}

std::variant<int, bool> DECL_REF_EXPR::execute()
{
    if(var_decl)
    {
        return var_decl->m_value;
    }
    return std::variant<int, bool>();
}

void DECL_REF_EXPR::setValue(const std::variant<int, bool>& v)
{
    if(var_decl)
    {
        var_decl->m_value = v;
    }
}

std::variant<int, bool> DECL_REF_EXPR::getValue() const
{
    if(var_decl)
    {
        return var_decl->m_value;
    }
    return std::variant<int, bool>();
}
