#ifndef IMPLICIT_CAST_EXPR_H
#define IMPLICIT_CAST_EXPR_H

#include "expr_node.h"

class IMPLICIT_CAST_EXPR : public EXPR_NODE
{
public:
    IMPLICIT_CAST_EXPR();

    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;
};

#endif // IMPLICIT_CAST_EXPR_H
