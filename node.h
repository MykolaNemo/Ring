#ifndef NODE_H
#define NODE_H

#include "clang/AST/ASTTypeTraits.h"

class NODE
{
public:
    virtual void addChild(NODE* child);
    
    std::vector<NODE*> children;
    clang::DynTypedNode astNode;
    NODE* parent = nullptr;
    
    // Support for LLVM-style RTTI (dyn_cast<> et al.)
public:
    enum NodeKind {
        NK_UndefinedNode,
        //Decl
        NK_FIRST_DECL,
        NK_VarDecl,
        NK_FunctionDecl,
        NK_LAST_DECL,

        NK_FIRST_STMT,
        NK_BinaryOp,
        NK_CompoundStmt,
        NK_DeclStmt,
        NK_ReturnStmt,

        NK_FIRST_EXPR,
        NK_IntLiteralExpr,
        NK_ImplicitCastExpr,
        NK_DeclRefExpr,
        NK_LAST_EXPR,

        NK_LAST_STMT,
        NK_LAST_NODE,
    };
    NODE();
    NodeKind getKind() const;
    static bool classof(const NODE *N);
    
protected:
    NODE(NodeKind K);
    
private:
    const NodeKind kind;
};

#endif // NODE_H
