#ifndef UNARY_OP_H
#define UNARY_OP_H

#include "stmt_node.h"

class UNARY_OP: public STMT_NODE
{
public:
    UNARY_OP(clang::UnaryOperator::Opcode opCode);

    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;

private:
    const clang::UnaryOperator::Opcode unaryOpCode;
};

#endif // UNARY_OP_H
