#include "return_stmt.h"
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

std::variant<int, bool> RETURN_STMT::execute()
{
    if(!children.empty())
    {
        return children.at(0)->execute();
    }
    return std::variant<int, bool>();
}
