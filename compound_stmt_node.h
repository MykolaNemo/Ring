#ifndef COMPOUND_STMT_NODE_H
#define COMPOUND_STMT_NODE_H

#include "stmt_node.h"

class COMPOUND_STMT_NODE: public STMT_NODE
{
public:
    COMPOUND_STMT_NODE();

    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;
};

#endif // COMPOUND_STMT_NODE_H
