#ifndef STMT_NODE_H
#define STMT_NODE_H

#include "node.h"

class STMT_NODE: public NODE
{
public:
    virtual int execute() = 0;

    static bool classof(const NODE *N)
    {
        return (N->getKind() > NK_FIRST_STMT)
        && (N->getKind() < NK_LAST_STMT);
    }

protected:
    STMT_NODE(NodeKind K) : NODE(K) {}
};

#endif // STMT_NODE_H
