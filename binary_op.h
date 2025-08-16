#ifndef BINARY_OP_H
#define BINARY_OP_H

#include "stmt_node.h"
#include "clang/AST/Expr.h"

class EXPR_NODE;

class BINARY_OP: public STMT_NODE
{
public:
    BINARY_OP(clang::BinaryOperator::Opcode opCode);

    static bool classof(const NODE *N);

    int execute() override;
    void addChild(NODE* child) override;

private:
    const clang::BinaryOperator::Opcode binOpCode;
    STMT_NODE* lhs = nullptr;
    STMT_NODE* rhs = nullptr;
};

#endif // BINARY_OP_H
