#ifndef DECL_STMT_H
#define DECL_STMT_H

#include "stmt_node.h"

class VAR_DECL;
class DECL_STMT : public STMT_NODE
{
public:
    DECL_STMT();
    static bool classof(const NODE *N);
    int execute() override;
    void addChild(NODE* child) override;
    
private:
    std::vector<VAR_DECL*> declarations;
};

#endif // DECL_STMT_H
