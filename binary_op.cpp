
#include "binary_op.h"
#include "decl_ref_expr.h"

#include <iostream>

using namespace clang;

BINARY_OP::BINARY_OP(BinaryOperator::Opcode opCode):
    STMT_NODE(NK_BinaryOp),
    binOpCode(opCode)
{

}

bool BINARY_OP::classof(const NODE *N)
{
    return N->getKind() == NK_BinaryOp;
}

int BINARY_OP::execute()
{
    if(binOpCode == BinaryOperator::Opcode::BO_Assign)
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            int rhsValue = dyn_cast<STMT_NODE>(rhs)->execute();
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            lhsVariable->setValue(rhsValue);
            return lhsVariable->execute();
        }
        else
        {
            std::cout<<"Binary Operator execute error"<<std::endl;
        }
        return 0;
    }
    else if(binOpCode == BinaryOperator::Opcode::BO_Add)
    {
        int lhsValue = dyn_cast<STMT_NODE>(lhs)->execute();
        int rhsValue = dyn_cast<STMT_NODE>(rhs)->execute();
        return lhsValue + rhsValue;
    }
    else if(binOpCode == BinaryOperator::Opcode::BO_Sub)
    {
        int lhsValue = dyn_cast<STMT_NODE>(lhs)->execute();
        int rhsValue = dyn_cast<STMT_NODE>(rhs)->execute();
        return lhsValue - rhsValue;
    }
    else if(binOpCode == BinaryOperator::Opcode::BO_Mul)
    {
        int lhsValue = dyn_cast<STMT_NODE>(lhs)->execute();
        int rhsValue = dyn_cast<STMT_NODE>(rhs)->execute();
        return lhsValue * rhsValue;
    }
    else if(binOpCode == BinaryOperator::Opcode::BO_Div)
    {
        int lhsValue = dyn_cast<STMT_NODE>(lhs)->execute();
        int rhsValue = dyn_cast<STMT_NODE>(rhs)->execute();
        return lhsValue / rhsValue;
    }
    else
    {
        std::cout<<"Binary Operator execute error"<<std::endl;
    }
    return 0;
}

void BINARY_OP::addChild(NODE *child)
{
    if(lhs && rhs)
    {
        std::cout<<"Can not assign more than 2 children to Binary operation"<<std::endl;
        return;
    }
    children.push_back(child);

    if(lhs == nullptr && isa<STMT_NODE>(child))
    {
        lhs = dyn_cast<STMT_NODE>(child);
    }
    else if(rhs == nullptr && isa<STMT_NODE>(child))
    {
        rhs = dyn_cast<STMT_NODE>(child);
    }
    else
    {
        std::cout<<"Binary operation, addChild error"<<std::endl;
    }
}
