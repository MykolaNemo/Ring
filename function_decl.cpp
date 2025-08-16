#include "function_decl.h"
#include "compound_stmt_node.h"
#include "var_decl.h"

#include <iostream>

using namespace clang;

FUNCTION_DECL::FUNCTION_DECL():
    NODE(NK_FunctionDecl)
{

}

bool FUNCTION_DECL::classof(const NODE *N)
{
    return N->getKind() == NK_FunctionDecl;
}

void FUNCTION_DECL::addChild(NODE *child)
{
    children.push_back(child);
    // if(isa<VAR_DECL>(child))
    // {
        // parameters->push_back(dyn_cast<VAR_DECL>(child));
    // }
    // else
    if(!functionBody && isa<COMPOUND_STMT_NODE>(child))
    {
        functionBody = dyn_cast<COMPOUND_STMT_NODE>(child);
    }
}

void FUNCTION_DECL::call()
{
    auto result = functionBody->execute();
    std::cout<<"Result: "<<result<<std::endl;
}
