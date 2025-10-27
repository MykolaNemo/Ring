#ifndef FUNCTION_DECL_H
#define FUNCTION_DECL_H

#include "decl_node.h"

class COMPOUND_STMT_NODE;
class FUNCTION_DECL: public DECL_NODE
{
public:
    FUNCTION_DECL();
    static bool classof(const NODE *N);
    void addChild(NODE* child) override;
    std::variant<int, bool> execute() override;

    std::string name;
    std::string returnType;
    COMPOUND_STMT_NODE* functionBody = nullptr;
};

#endif // FUNCTION_DECL_H
