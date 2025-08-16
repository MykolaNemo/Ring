#include "decl_stmt.h"
#include "var_decl.h"
#include <iostream>

using namespace clang;

DECL_STMT::DECL_STMT() :
    STMT_NODE(NK_DeclStmt)
{

}

bool DECL_STMT::classof(const NODE *N)
{
    return N->getKind() == NK_DeclStmt;
}

int DECL_STMT::execute()
{
    return 0;
}

void DECL_STMT::addChild(NODE *child)
{
    children.push_back(child);
    if(isa<VAR_DECL>(child))
    {
        declarations.push_back(dyn_cast<VAR_DECL>(child));
    }
}
