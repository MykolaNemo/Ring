#include "return_stmt.h"
#include "expr_node.h"
#include <iostream>

using namespace clang;

RETURN_STMT::RETURN_STMT():
    STMT_NODE(NK_ReturnStmt)
{

}

bool RETURN_STMT::classof(const NODE *N)
{
    return N->getKind() == NK_ReturnStmt;
}

int RETURN_STMT::execute()
{
    if(return_expr)
    {
        return return_expr->execute();
    }
    return 0;
}

void RETURN_STMT::addChild(NODE *child)
{
    children.push_back(child);
    if(!return_expr && isa<EXPR_NODE>(child))
    {
        return_expr = dyn_cast<EXPR_NODE>(child);
    }
}
