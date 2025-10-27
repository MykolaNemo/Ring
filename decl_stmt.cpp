#include "decl_stmt.h"
#include "var_decl.h"
#include <iostream>

using namespace clang;

DECL_STMT::DECL_STMT() :
    STMT_NODE(NK_DeclStmt)
{

}

DECL_STMT::DECL_STMT(NodeKind kind) :
    STMT_NODE(kind)
{

}

bool DECL_STMT::classof(const NODE *N)
{
    return N->getKind() == NK_DeclStmt;
}

std::variant<int, bool> DECL_STMT::execute()
{
    for(NODE* node : children)
    {
        node->execute();
    }
    return std::variant<int, bool>();
}
