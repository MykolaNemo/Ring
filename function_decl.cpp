#include "function_decl.h"
#include "compound_stmt_node.h"

using namespace clang;

FUNCTION_DECL::FUNCTION_DECL():
    DECL_NODE(NK_FunctionDecl)
{

}

bool FUNCTION_DECL::classof(const NODE *N)
{
    return N->getKind() == NK_FunctionDecl;
}

void FUNCTION_DECL::addChild(NODE *child)
{
    children.push_back(child);
    if(!functionBody && isa<COMPOUND_STMT_NODE>(child))
    {
        functionBody = dyn_cast<COMPOUND_STMT_NODE>(child);
    }
}

std::variant<int, bool> FUNCTION_DECL::execute()
{
    return functionBody->execute();
}
