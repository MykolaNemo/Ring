#ifndef FUNCTION_DECL_H
#define FUNCTION_DECL_H

#include "node.h"
#include <memory>

class COMPOUND_STMT_NODE;
class VAR_DECL;
class FUNCTION_DECL: public NODE
{
public:
    FUNCTION_DECL();
    static bool classof(const NODE *N);
    void addChild(NODE* child) override;
    void call();

    std::string name;
    std::string returnType;
    // std::shared_ptr<Scope> parameters;
    COMPOUND_STMT_NODE* functionBody = nullptr;
};

#endif // FUNCTION_DECL_H
