#include "ASTCrawlerAction.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/Casting.h"

#include <iostream>
#include <string>
#include <vector>

using namespace clang;

std::string parseExpression(Expr* expr, ASTContext* Context)
{
    if(isa<IntegerLiteral>(expr))
    {
        Expr::EvalResult eval_result;
        expr->EvaluateAsInt(eval_result, *Context);
        return std::to_string(*eval_result.Val.getInt().getRawData());
    }
    else
    {
        std::cout<<"parseExpression error"<<std::endl;
    }
    return {};
}

std::string parseTypeName(QualType type)
{
    if(type->isBuiltinType())
    {
        BuiltinType const* builtin_type = static_cast<BuiltinType const*>(type.getTypePtr());
        return builtin_type->getName(PrintingPolicy(LangOptions())).data();
    }
    else if(type->isIncompleteArrayType())
    {
        IncompleteArrayType const* c_array_type = static_cast<IncompleteArrayType const*>(type.getTypePtr());
        return std::string(parseTypeName(c_array_type->getElementType())) + "[]";
    }
    else if(type->isPointerType())
    {
        PointerType const* pointer_type = static_cast<PointerType const*>(type.getTypePtr());
        return std::string(parseTypeName(pointer_type->getPointeeType())) + "*";
    }
    return std::string("[TypeClassName:") + type->getTypeClassName() + "]";
}

// class AnyType
// {
//     // Support for LLVM-style RTTI (dyn_cast<> et al.)
// public:
//     enum TypeKind
//     {
//         AT_Undefined,
//         AT_Int,
//         AT_LAST_TYPE
//     };
//     AnyType() : kind(AT_Undefined) {}
//     TypeKind getTypeKind() const { return kind; }
//     static bool classof(const AnyType *N)
//     {
//         return (N->getTypeKind() >= AT_Undefined)
//             && (N->getTypeKind() < AT_LAST_TYPE);
//     }

// protected:
//     AnyType(TypeKind K) : kind(K) {}

// private:
//     const TypeKind kind;
// };

// class IntType
// };

namespace{

class EXPR_NODE;

class NODE
{
public:
    virtual void addChild(NODE* child)
    {
        children.push_back(child);
    }
    DynTypedNode astNode;
    NODE* parent = nullptr;

protected:
    std::vector<NODE*> children;

// Support for LLVM-style RTTI (dyn_cast<> et al.)
public:
    enum NodeKind {
        NK_UndefinedNode,
        //Decl
        NK_VarDecl,
        NK_FunctionDecl,
        NK_LAST_DECL,
        //Stmt
        NK_CompoundStmt,
        NK_BinaryOp,
        NK_LAST_STMT,
        //Expr
        NK_IntLiteralExpr,
        NK_ImplicitCastExpr,
        NK_DeclRefExpr,
        NK_LAST_EXPR,
        NK_LAST_NODE,
    };
    NODE() : kind(NK_UndefinedNode) {}
    NodeKind getKind() const { return kind; }
    static bool classof(const NODE *N)
    {
        return (N->getKind() >= NK_UndefinedNode)
            && (N->getKind() < NK_LAST_NODE);
    }

protected:
    NODE(NodeKind K) : kind(K) {}

private:
    const NodeKind kind;
};

class VAR_DECL_NODE: public NODE
{
public:
    VAR_DECL_NODE(): NODE(NK_VarDecl){}
    std::string name;
    std::string type;
    std::string m_value;
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_VarDecl;
    }
};

class FUNCTION_DECL_NODE: public NODE
{
public:
    FUNCTION_DECL_NODE(): NODE(NK_FunctionDecl){}
    std::string name;
    std::string returnType;
    std::vector<VAR_DECL_NODE*> parameters;
    void addChild(NODE* child) override
    {
        children.push_back(child);
        if(isa<VAR_DECL_NODE>(child))
        {
            parameters.push_back(dyn_cast<VAR_DECL_NODE>(child));
        }
    }
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_FunctionDecl;
    }
};

class STMT_NODE: public NODE
{
public:
    virtual std::string execute() = 0;

protected:
    STMT_NODE(NodeKind K) : NODE(K) {}
};

class EXPR_NODE: public NODE
{
public:
    virtual std::string value() const = 0;

protected:
    EXPR_NODE(NodeKind K) : NODE(K) {}
};

class COMPOUND_STMT_NODE: public STMT_NODE
{
public:
    COMPOUND_STMT_NODE(): STMT_NODE(NK_CompoundStmt){}
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_CompoundStmt;
    }

    std::vector<VAR_DECL_NODE*> localVars;
    void addChild(NODE* child) override
    {
        children.push_back(child);
        if(isa<VAR_DECL_NODE>(child))
        {
            localVars.push_back(dyn_cast<VAR_DECL_NODE>(child));
        }
    }

    std::string execute() override
    {
        for(NODE* stmt : children)
        {
            static_cast<STMT_NODE*>(stmt)->execute();
        }
        return std::string();
    }
};


class DECL_REF_EXPR : public EXPR_NODE
{
public:
    DECL_REF_EXPR(): EXPR_NODE(NK_DeclRefExpr){}
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_DeclRefExpr;
    }

    std::string value() const override
    {
        if(decl)
        {
            return decl->m_value;
        }
        return "";
    }

    void setValue(const std::string& v)
    {
        decl->m_value = v;
    }

    VAR_DECL_NODE* decl = nullptr;
};


class BINARY_OP_NODE: public STMT_NODE
{
public:
    enum BinaryOpKind
    {
        BO_Assignment,
        BO_Add
    };
    BINARY_OP_NODE(BinaryOpKind opKind): STMT_NODE(NK_BinaryOp), binOpKind(opKind) {}
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_BinaryOp;
    }

    std::string execute() override
    {
        if(binOpKind == BO_Assignment)
        {
            if(isa<DECL_REF_EXPR>(lhs))
            {
                dyn_cast<DECL_REF_EXPR>(lhs)->setValue(rhs->value());
            }
            else
            {
                std::cout<<"Binary Operator execute error"<<std::endl;
            }
            return lhs->value();
        }
        else if(binOpKind == BO_Add)
        {
            return lhs->value() + rhs->value();
        }
        else
        {
            std::cout<<"Binary Operator execute error"<<std::endl;
        }
        return std::string();
    }

    void addChild(NODE* child) override
    {
        if(lhs && rhs)
        {
            std::cout<<"Can not assign more than 2 children to Binary operation"<<std::endl;
            return;
        }
        children.push_back(child);
        if(binOpKind == BO_Assignment)
        {
            if((lhs == nullptr) && isa<DECL_REF_EXPR>(child))
            {
                lhs = dyn_cast<DECL_REF_EXPR>(child);
            }
            else if((rhs == nullptr) && isa<EXPR_NODE>(child))
            {
                rhs = dyn_cast<EXPR_NODE>(child);
            }
        }
        else if (binOpKind == BO_Add)
        {
            if((lhs == nullptr) && isa<EXPR_NODE>(child))
            {
                lhs = dyn_cast<EXPR_NODE>(child);
            }
            else if((rhs == nullptr) && isa<EXPR_NODE>(child))
            {
                rhs = dyn_cast<EXPR_NODE>(child);
            }
        }
        else
        {
            std::cout<<"Can not assign a child to Binary operation: wrong type"<<std::endl;
        }
    }

    const BinaryOpKind binOpKind;
    EXPR_NODE* lhs = nullptr;
    EXPR_NODE* rhs = nullptr;
};

class INT_LITERAL_EXPR : public EXPR_NODE
{
public:
    INT_LITERAL_EXPR(int _v = 0):EXPR_NODE(NK_IntLiteralExpr),v(_v){}
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_IntLiteralExpr;
    }
    std::string value() const override
    {
        return std::to_string(v);
    };

private:
    int v;
};

class IMPLICIT_CAST_EXPR : public EXPR_NODE
{
public:
    IMPLICIT_CAST_EXPR(): EXPR_NODE(NK_ImplicitCastExpr){}
    static bool classof(const NODE *N)
    {
        return N->getKind() == NK_ImplicitCastExpr;
    }
    void addChild(NODE* child) override
    {
        children.push_back(child);
        if(isa<DECL_REF_EXPR>(child))
        {
            decl = dyn_cast<DECL_REF_EXPR>(child);
        }
    }
    std::string value() const override
    {
        if(decl)
        {
            return decl->value();
        }
        std::cout<<"IMPLICIT_CAST_EXPR: can not return value"<<std::endl;
        return "";
    }
    DECL_REF_EXPR* decl = nullptr;
};

NODE* translationUnitNode = nullptr;
std::vector<NODE*> allNodes;
std::vector<VAR_DECL_NODE*> allVariables;

NODE* findDataNodeByASTNode(DynTypedNode astNode)
{
    auto it = std::find_if(allNodes.begin(), allNodes.end(), [&astNode](NODE* node)->bool{
        return node->astNode == astNode;
    });
    if(it != allNodes.end())
    {
        return *it;
    }
    return nullptr;
}

// VAR_DECL_NODE* findVariableByName(const std::string& name)
// {
//     auto it = std::find_if(allVariables.begin(), allVariables.end(), [name](VAR_DECL_NODE* var)->bool{
//         return var->name == name;
//     });
//     if(it != allVariables.end())
//     {
//         return *it;
//     }
//     return nullptr;
// }

template <typename NodeT>
inline bool addNode(NODE* dataNode, ASTContext* context, const NodeT &ASTNode)
{
    dataNode->astNode = DynTypedNode::create(ASTNode);
    allNodes.push_back(dataNode);

    auto parentsList = context->getParents(ASTNode);
    if(parentsList.size() == 1)
    {
        auto parentASTNode = parentsList[0];

        // Skip TypeLoc nodes
        const TypeLoc* typeLocation = parentASTNode.template get<TypeLoc>();
        while(typeLocation)
        {
            parentASTNode = context->getParents(*typeLocation)[0];
            typeLocation = parentASTNode.template get<TypeLoc>();
        }

        NODE* parentDataNode = findDataNodeByASTNode(parentASTNode);
        if(parentDataNode)
        {
            std::cout<<"+";
            parentDataNode->addChild(dataNode);
            dataNode->parent = parentDataNode;
            return true;
        }
        std::cout<<"-";
    }
    else if(parentsList.size() > 1)
    {
        std::cout<<"Multiple parents!"<<std::endl;
    }
    return false;
}

}

template<class NodeT>
int numOfParents(ASTContext* context, const NodeT& ASTNode)
{
    int n = 0;
    auto parentsList = context->getParents(ASTNode);
    while(parentsList.size() != 0)
    {
        n++;
        parentsList = context->getParents(parentsList[0]);
    }
    return n;
}

ASTCrawlerVisitor::ASTCrawlerVisitor(ASTContext *Context)
    : Context(Context)
{
}

bool ASTCrawlerVisitor::TraverseDecl(Decl *D)
{
    if(!D) return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseDecl(D);

    int parentsCount = numOfParents(Context, *D);
    std::cout<<parentsCount<<":";
    for(int i = 0; i < parentsCount; ++i)
    {
        std::cout<<"...";
    }

    if(llvm::isa<TranslationUnitDecl>(D))
    {
        std::cout<<"+";
        translationUnitNode = new NODE();
        addNode(translationUnitNode, Context, *D);
    }
    else if(isa<FunctionDecl>(D))
    {
        std::cout<<"+";
        FUNCTION_DECL_NODE* functionDeclNode = new FUNCTION_DECL_NODE();

        FunctionDecl* func_decl = static_cast<FunctionDecl*>(D);
        functionDeclNode->returnType = parseTypeName(func_decl->getReturnType());
        functionDeclNode->name = func_decl->getName().data();

        addNode(functionDeclNode, Context, *D);
    }
    else if(isa<ParmVarDecl>(D))
    {
        std::cout<<"+";
        VAR_DECL_NODE* varDataNode = new VAR_DECL_NODE();

        ParmVarDecl* param_decl = static_cast<ParmVarDecl*>(D);
        varDataNode->name = param_decl->getName().data();
        varDataNode->type = parseTypeName(param_decl->getOriginalType());

        addNode(varDataNode, Context, *D);
    }
    std::cout<<"[Decl]: "<<D->getDeclKindName()<<std::endl;
    RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseDecl(D);
    return true;
}

bool ASTCrawlerVisitor::TraverseStmt(Stmt *x, DataRecursionQueue *Queue)
{
    if(!x) return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseStmt(x, Queue);

    bool printName = true;
    std::string paddingDebugString;
    int parentsCount = numOfParents(Context, *x);
    paddingDebugString += std::to_string(parentsCount) + ":";
    for(int i = 0; i < parentsCount; ++i)
    {
        paddingDebugString += "...";
    }

    if(isa<CompoundStmt>(x))
    {
        std::cout<<paddingDebugString<<"+";
        addNode(new COMPOUND_STMT_NODE(), Context, *x);
    }
    else if(isa<DeclStmt>(x))
    {
        printName = false;
        DeclStmt* decl_stmt = static_cast<DeclStmt*>(x);
        std::vector<Decl*> declarations;
        if(decl_stmt->isSingleDecl())
        {
            declarations.push_back(decl_stmt->getSingleDecl());
        }
        else
        {
            for(auto it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); ++it)
            {
                declarations.push_back(*it);
            }
        }
        for(Decl* decl : declarations)
        {
            if(isa<VarDecl>(decl))
            {
                std::cout<<paddingDebugString<<"+";
                VAR_DECL_NODE* varDataNode = new VAR_DECL_NODE();

                VarDecl* var_decl = static_cast<VarDecl*>(decl);
                varDataNode->name = var_decl->getNameAsString();
                varDataNode->type = parseTypeName(var_decl->getType());

                addNode(varDataNode, Context, *x);

                Expr* initExpr = var_decl->getInit();
                if(initExpr)
                {
                    varDataNode->m_value = parseExpression(initExpr, Context);
                }
                std::cout<<"[Stmt]: "<<x->getStmtClassName()<<std::endl;
            }
            else
            {
                std::cout<<"-";
                addNode(new NODE(), Context, *x);
            }
        }
        return true;
    }
    else if(isa<BinaryOperator>(x))
    {
        BinaryOperator* bin_op = static_cast<BinaryOperator*>(x);
        if(bin_op->getOpcode() == BO_Assign)
        {
            std::cout<<paddingDebugString<<"+";
            BINARY_OP_NODE* bin_op_node = new BINARY_OP_NODE(BINARY_OP_NODE::BO_Assignment);
            addNode(bin_op_node, Context, *x);
        }
        else if(bin_op->getOpcode() == BO_Add)
        {
            std::cout<<paddingDebugString<<"+";
            BINARY_OP_NODE* bin_op_node = new BINARY_OP_NODE(BINARY_OP_NODE::BO_Add);
            addNode(bin_op_node, Context, *x);
        }
        else
        {
            std::cout<<"-";
        }
    }
    else if(isa<DeclRefExpr>(x))
    {
        std::cout<<paddingDebugString<<"+";
        addNode(new DECL_REF_EXPR(), Context, *dyn_cast<DeclRefExpr>(x));
    }
    else if(isa<ImplicitCastExpr>(x))
    {
        std::cout<<paddingDebugString<<"+";
        addNode(new IMPLICIT_CAST_EXPR(), Context, *dyn_cast<ImplicitCastExpr>(x));
    }
    else if(isa<IntegerLiteral>(x))
    {
        std::cout<<paddingDebugString<<"+";

        IntegerLiteral* int_literal = dyn_cast<IntegerLiteral>(x);
        Expr::EvalResult eval_result;
        int intValue = int_literal->EvaluateAsInt(eval_result, *Context);

        addNode(new INT_LITERAL_EXPR(intValue), Context, *int_literal);
    }

    if(printName)
    {
        std::cout<<"[Stmt]: "<<x->getStmtClassName()<<std::endl;
    }
    RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseStmt(x, Queue);
    return true;
}

bool ASTCrawlerVisitor::TraverseType(QualType x)
{
    if(!x.isNull())
    {
        std::cout<<"[Type]: "<<x->getTypeClassName()<<std::endl;
    }
    RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseType(x);
    return true;
}

ASTCrawlerConsumer::ASTCrawlerConsumer(ASTContext *Context)
    : Visitor(Context)
{
}

void ASTCrawlerConsumer::HandleTranslationUnit(clang::ASTContext &Context)
{
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}

std::unique_ptr<clang::ASTConsumer> ASTCrawlerAction::CreateASTConsumer(
                                    clang::CompilerInstance &Compiler, llvm::StringRef InFile)
{
    return std::make_unique<ASTCrawlerConsumer>(&Compiler.getASTContext());
}

// bool ASTCrawlerVisitor::TraverseAttr (Attr *At)
// {
//     std::cout<<"[TraverseAttr]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseAttr(At);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseNestedNameSpecifier (NestedNameSpecifier *NNS)
// {
//     std::cout<<"[TraverseNestedNameSpecifier]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseNestedNameSpecifier(NNS);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseNestedNameSpecifierLoc (NestedNameSpecifierLoc NNS)
// {
//     if(!NNS) return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseNestedNameSpecifierLoc(NNS);

//     std::cout<<"[TraverseNestedNameSpecifierLoc] ";
//     std::cout<<NNS.getNestedNameSpecifier()->getKind()<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseNestedNameSpecifierLoc(NNS);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseDeclarationNameInfo (DeclarationNameInfo NameInfo)
// {
//     std::cout<<"[TraverseDeclarationNameInfo]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseDeclarationNameInfo(NameInfo);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseTemplateName (TemplateName Template)
// {
//     std::cout<<"[TraverseTemplateName]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseTemplateName(Template);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseTemplateArgument (const TemplateArgument &Arg)
// {
//     std::cout<<"[TraverseTemplateArgument]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseTemplateArgument(Arg);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseTemplateArgumentLoc (const TemplateArgumentLoc &ArgLoc)
// {
//     std::cout<<"[TraverseTemplateArgumentLoc]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseTemplateArgumentLoc(ArgLoc);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseTemplateArguments (ArrayRef< TemplateArgument > Args)
// {
//     std::cout<<"[TraverseTemplateArguments]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseTemplateArguments(Args);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseCXXBaseSpecifier (const CXXBaseSpecifier &Base)
// {
//     std::cout<<"[TraverseCXXBaseSpecifier]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseCXXBaseSpecifier(Base);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseConstructorInitializer (CXXCtorInitializer *Init)
// {
//     std::cout<<"[TraverseConstructorInitializer]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseConstructorInitializer(Init);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseLambdaCapture (LambdaExpr *LE, const LambdaCapture *C, Expr *Init)
// {
//     std::cout<<"[TraverseLambdaCapture]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseLambdaCapture(LE, C, Init);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseSynOrSemInitListExpr (InitListExpr *S, DataRecursionQueue *Queue)
// {
//     std::cout<<"[TraverseSynOrSemInitListExpr]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseSynOrSemInitListExpr(S, Queue);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseObjCProtocolLoc (ObjCProtocolLoc ProtocolLoc)
// {
//     std::cout<<"[TraverseObjCProtocolLoc]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseObjCProtocolLoc(ProtocolLoc);
//     // return true;
// }

// bool ASTCrawlerVisitor::TraverseConceptReference (ConceptReference *CR)
// {
//     std::cout<<"[TraverseConceptReference]"<<std::endl;
//     return RecursiveASTVisitor<ASTCrawlerVisitor>::TraverseConceptReference(CR);
//     // return true;
// }
