#ifndef EXPR_NODE_H
#define EXPR_NODE_H

#include "stmt_node.h"

class EXPR_NODE: public STMT_NODE
{
public:
    static bool classof(const NODE *N)
    {
        return (N->getKind() > NK_FIRST_EXPR)
            && (N->getKind() < NK_LAST_EXPR);
    }
    
protected:
    EXPR_NODE(NodeKind K) : STMT_NODE(K) {}
};

#endif // EXPR_NODE_H
