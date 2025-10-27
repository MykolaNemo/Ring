
#include "unary_op.h"

#include <iostream>
#include <variant>

using namespace clang;

UNARY_OP::UNARY_OP(UnaryOperator::Opcode opCode):
    STMT_NODE(NK_UnaryOp),
    unaryOpCode(opCode)
{

}

bool UNARY_OP::classof(const NODE *N)
{
    return N->getKind() == NK_UnaryOp;
}

struct Visitor
{
    // all other overloads invalid
    template <class T>
    auto operator()(T) const -> std::variant<int,bool>
    {
        std::cout<<"Unary operator visitor failed"<<std::endl;
        return {};
    }
};

struct MinusVisitor : public Visitor
{
    auto operator()(int a) const -> std::variant<int,bool>
    {
        return -a;
    }
};

struct PlusVisitor : public Visitor
{
    auto operator()(int a) const -> std::variant<int,bool>
    {
        return +a;
    }
};

std::variant<int, bool> UNARY_OP::execute()
{
    if(children.empty())
    {
        return std::variant<int, bool>();
    }
    auto child = children.at(0);
    switch(unaryOpCode)
    {
    case UnaryOperator::Opcode::UO_Minus:
    {
        const auto& childValue = child->execute();
        return std::visit(MinusVisitor(), childValue);
    }
    case UnaryOperator::Opcode::UO_Plus:
    {
        const auto& childValue = child->execute();
        return std::visit(PlusVisitor(), childValue);
    }
    default:
    {
        std::cout<<"Unary Operator execute() error"<<std::endl;
    }
    }
    return std::variant<int, bool>();
}
