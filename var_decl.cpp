#include "var_decl.h"
#include <iostream>

VAR_DECL::VAR_DECL():
    DECL_NODE(NK_VarDecl)
{

}

bool VAR_DECL::classof(const NODE *N)
{
    return N->getKind() == NK_VarDecl;
}

std::variant<int, bool> VAR_DECL::execute()
{
    for(NODE* child : children)
    {
        m_value = child->execute();
    }
    return std::variant<int, bool>();
}
