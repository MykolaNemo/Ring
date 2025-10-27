#ifndef PAREN_EXPR_H
#define PAREN_EXPR_H

#include "expr_node.h"

class PAREN_EXPR : public EXPR_NODE
{
public:
    PAREN_EXPR();

    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;
};

#endif // PAREN_EXPR_H
