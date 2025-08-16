#include "var_decl.h"

VAR_DECL::VAR_DECL():
    NODE(NK_VarDecl)
{

}

bool VAR_DECL::classof(const NODE *N)
{
    return N->getKind() == NK_VarDecl;
}
