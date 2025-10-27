#ifndef DECL_REF_EXPR_H
#define DECL_REF_EXPR_H

#include "expr_node.h"
#include "var_decl.h"
// #include "decl_node.h"

class VAR_DECL;
// template<typename T>
class DECL_REF_EXPR : public EXPR_NODE
{
public:
    DECL_REF_EXPR();

    static bool classof(const NODE *N);
    void addChild(NODE* child) override;
    
    std::variant<int, bool> execute() override;

    void setValue(const std::variant<int, bool> &v);
    std::variant<int, bool> getValue() const;

    VAR_DECL* var_decl = nullptr;
};

#endif // DECL_REF_EXPR_H
