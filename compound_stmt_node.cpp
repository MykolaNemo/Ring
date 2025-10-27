#include "compound_stmt_node.h"
#include "return_stmt.h"

using namespace clang;

COMPOUND_STMT_NODE::COMPOUND_STMT_NODE():
    STMT_NODE(NK_CompoundStmt)
{

}

bool COMPOUND_STMT_NODE::classof(const NODE *N)
{
    return N->getKind() == NK_CompoundStmt;
}

std::variant<int, bool> COMPOUND_STMT_NODE::execute()
{
    for(NODE* child : children)
    {
        if(isa<RETURN_STMT>(child))
        {
            return child->execute();
        }
        else
        {
            child->execute();
        }
    }
    return std::variant<int, bool>();
}
