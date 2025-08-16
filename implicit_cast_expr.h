#ifndef IMPLICIT_CAST_EXPR_H
#define IMPLICIT_CAST_EXPR_H

#include "expr_node.h"

class DECL_REF_EXPR;
class IMPLICIT_CAST_EXPR : public EXPR_NODE
{
public:
    IMPLICIT_CAST_EXPR();

    static bool classof(const NODE *N);

    void addChild(NODE* child) override;

    int execute() override;

private:
    DECL_REF_EXPR* decl = nullptr;
};

#endif // IMPLICIT_CAST_EXPR_H
