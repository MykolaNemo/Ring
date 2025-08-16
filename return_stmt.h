#ifndef RETURN_STMT_H
#define RETURN_STMT_H

#include "stmt_node.h"

class EXPR_NODE;
class RETURN_STMT : public STMT_NODE
{
public:
    RETURN_STMT();
    static bool classof(const NODE *N);
    
    int execute() override;
    
    void addChild(NODE* child) override;
    
    EXPR_NODE* return_expr = nullptr;
};

#endif // RETURN_STMT_H
