#ifndef INT_LITERAL_EXPR_H
#define INT_LITERAL_EXPR_H

#include "expr_node.h"

class INT_LITERAL_EXPR : public EXPR_NODE
{
public:
    INT_LITERAL_EXPR(int _v = 0);

    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;
    
private:
    int v;
};

#endif // INT_LITERAL_EXPR_H
