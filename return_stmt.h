#ifndef RETURN_STMT_H
#define RETURN_STMT_H

#include "stmt_node.h"

class RETURN_STMT : public STMT_NODE
{
public:
    RETURN_STMT();
    static bool classof(const NODE *N);
    
    std::variant<int, bool> execute() override;
};

#endif // RETURN_STMT_H
