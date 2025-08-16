#include "compound_stmt_node.h"
#include "var_decl.h"
#include "return_stmt.h"
#include <iostream>

using namespace clang;

COMPOUND_STMT_NODE::COMPOUND_STMT_NODE():
    STMT_NODE(NK_CompoundStmt)
{

}

bool COMPOUND_STMT_NODE::classof(const NODE *N)
{
    return N->getKind() == NK_CompoundStmt;
}

// void COMPOUND_STMT_NODE::addChild(NODE *child)
// {
//     children.push_back(child);
//     if(isa<VAR_DECL>(child))
//     {
//         localVars.push_back(dyn_cast<VAR_DECL>(child));
//     }
// }

int COMPOUND_STMT_NODE::execute()
{
    int result = 0;
    for(NODE* child : children)
    {
        if(isa<RETURN_STMT>(child))
        {
            result = dyn_cast<RETURN_STMT>(child)->execute();
            break;
        }
        else if(isa<STMT_NODE>(child))
        {
            dyn_cast<STMT_NODE>(child)->execute();
        }
        else
        {
            std::cout<<"COMPOUND_STMT_NODE: child is not a Statement"<<std::endl;
        }
    }
    return result;
}
