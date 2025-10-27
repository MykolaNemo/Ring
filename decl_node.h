#ifndef DECL_NODE_H
#define DECL_NODE_H

#include "node.h"

class DECL_NODE : public NODE
{
public:
    DECL_NODE() = delete;
    virtual ~DECL_NODE() = default;
    static bool classof(const NODE *N)
    {
        return (N->getKind() > NK_FIRST_DECL)
               && (N->getKind() < NK_LAST_DECL);
    }

protected:
    DECL_NODE(NodeKind K) : NODE(K) {}
};

#endif // DECL_NODE_H
