
#include "binary_op.h"
#include "decl_ref_expr.h"

#include <iostream>
#include <variant>

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

struct Visitor
{
    // all other overloads invalid
    template <class T, class U>
    auto operator()(T, U) const -> std::variant<int,bool>
    {
        std::cout<<"Binary operator visitor failed"<<std::endl;
        return {};
    }
};

struct AddVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a + b;
    }
};


struct SubVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a - b;
    }
};

struct MulVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a * b;
    }
};

struct DivVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a / b;
    }
};

struct RemVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a % b;
    }
};

struct ShlVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a << b;
    }
};

struct ShrVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a >> b;
    }
};

struct AndVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a & b;
    }
};

struct OrVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a | b;
    }
};

struct XorVisitor : public Visitor
{
    auto operator()(int a, int b) const -> std::variant<int,bool>
    {
        return a ^ b;
    }
};

std::variant<int, bool> BINARY_OP::execute()
{
    switch(binOpCode)
    {
    case BinaryOperator::Opcode::BO_Assign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            lhsVariable->setValue(rhs->execute());
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_MulAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            lhsVariable->getValue();
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(MulVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_DivAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(DivVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_RemAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(RemVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_AddAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(AddVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_SubAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(SubVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_ShlAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(ShlVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_ShrAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(ShrVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_AndAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(AndVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_XorAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(XorVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_OrAssign:
    {
        if(isa<DECL_REF_EXPR>(lhs))
        {
            DECL_REF_EXPR* lhsVariable = dyn_cast<DECL_REF_EXPR>(lhs);
            const auto& lhsValue = lhsVariable->getValue();
            const auto& rhsValue = rhs->execute();
            lhsVariable->setValue(std::visit(OrVisitor(), lhsValue, rhsValue));
            return lhs->execute();
        }
        break;
    }
    case BinaryOperator::Opcode::BO_Add:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(AddVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Sub:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(SubVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Mul:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(MulVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Div:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(DivVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Rem:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(RemVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Shl:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(ShlVisitor(), lhsValue, rhsValue);
    }
    case BinaryOperator::Opcode::BO_Shr:
    {
        const auto& lhsValue = lhs->execute();
        const auto& rhsValue = rhs->execute();
        return std::visit(ShrVisitor(), lhsValue, rhsValue);
    }
    //c++20
    // else if(binOpCode == BinaryOperator::Opcode::BO_Cmp)
    // {
    // return lhs->execute() <=> rhs->execute();
    // }
    default:
    {
        std::cout<<"Binary Operator execute() error"<<std::endl;
    }
    }
    return std::variant<int, bool>();
}

void BINARY_OP::addChild(NODE *child)
{
    if(lhs && rhs)
    {
        std::cout<<"Can not assign more than 2 children to Binary operation"<<std::endl;
        return;
    }

    children.push_back(child);
    if(!lhs)
    {
        lhs = child;
    }
    else if(!rhs)
    {
        rhs = child;
    }
}
