#ifndef DECL_REF_EXPR_H
#define DECL_REF_EXPR_H

#include "expr_node.h"

class VAR_DECL;
class DECL_REF_EXPR : public EXPR_NODE
{
public:
    DECL_REF_EXPR();

    static bool classof(const NODE *N);
    void addChild(NODE* child) override;
    
    int execute() override;
    void setValue(int v);

    VAR_DECL* decl = nullptr;
// private:
};

#endif // DECL_REF_EXPR_H
