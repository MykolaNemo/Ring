#include "ASTCrawlerAction.h"
#include "node.h"
#include "binary_op.h"
#include "compound_stmt_node.h"
#include "decl_ref_expr.h"
#include "decl_stmt.h"
#include "expr_node.h"
#include "function_decl.h"
#include "implicit_cast_expr.h"
#include "int_literal_expr.h"
#include "return_stmt.h"
#include "stmt_node.h"
#include "var_decl.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "executor.h"
// #include "llvm/Support/Casting.h"

#include <iostream>
#include <string>
#include <vector>

using namespace clang;

namespace{

NODE* translationUnitNode = nullptr;
FUNCTION_DECL* mainFunctionNode = nullptr;
std::vector<NODE*> allNodes;
std::vector<VAR_DECL*> allVariables;

int parseExpression(Expr* expr, ASTContext* Context)
{
    if(isa<IntegerLiteral>(expr))
    {
        Expr::EvalResult eval_result;
        expr->EvaluateAsInt(eval_result, *Context);
        std::cout<<"integer: "<<*eval_result.Val.getInt().getRawData()<<std::endl;
        return *eval_result.Val.getInt().getRawData();
    }
    else
    {
        std::cout<<"parseExpression error for "<<expr->getStmtClassName()<<std::endl;
    }
    return 0;
}

std::string parseTypeName(QualType type)
{
    if(type->isBuiltinType())
    {
        BuiltinType const* builtin_type = dyn_cast<BuiltinType>(type.getTypePtr());
        return builtin_type->getName(PrintingPolicy(LangOptions())).data();
    }
    else if(type->isIncompleteArrayType())
    {
        IncompleteArrayType const* c_array_type = dyn_cast<IncompleteArrayType>(type.getTypePtr());
        return std::string(parseTypeName(c_array_type->getElementType())) + "[]";
    }
    else if(type->isPointerType())
    {
        PointerType const* pointer_type = dyn_cast<PointerType>(type.getTypePtr());
        return std::string(parseTypeName(pointer_type->getPointeeType())) + "*";
    }
    return std::string("[TypeClassName:") + type->getTypeClassName() + "]";
}

void dumpTranslationUnit(ASTContext* context)
{
    std::string str;
    llvm::raw_string_ostream  stream(str);
    translationUnitNode->astNode.dump(stream, *context);
    std::cout<<str<<std::endl;
}

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
        std::cout<<"addNode: can not find parent"<<std::endl;
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

    if(isa<TranslationUnitDecl>(D))
    {
        std::cout<<"+";
        translationUnitNode = new NODE();
        addNode(translationUnitNode, Context, *D);
    }
    else if(isa<FunctionDecl>(D))
    {
        std::cout<<"+";
        FUNCTION_DECL* functionDeclNode = new FUNCTION_DECL();

        FunctionDecl* func_decl = dyn_cast<FunctionDecl>(D);
        functionDeclNode->returnType = parseTypeName(func_decl->getReturnType());
        functionDeclNode->name = func_decl->getName().data();
        if(functionDeclNode->name == "main")
        {
            mainFunctionNode = functionDeclNode;
        }

        addNode(functionDeclNode, Context, *D);
    }
    else if(isa<ParmVarDecl>(D))
    {
        std::cout<<"+";
        VAR_DECL* varDataNode = new VAR_DECL();

        ParmVarDecl* param_decl = dyn_cast<ParmVarDecl>(D);
        varDataNode->name = param_decl->getName().data();
        varDataNode->type = parseTypeName(param_decl->getOriginalType());

        addNode(varDataNode, Context, *D);
    }
    else if(isa<VarDecl>(D))
    {
        std::cout<<"+";

        VarDecl* var_decl = dyn_cast<VarDecl>(D);

        VAR_DECL* varDataNode = new VAR_DECL();
        varDataNode->name = var_decl->getName().data();
        varDataNode->type = parseTypeName(var_decl->getType());
        Expr* init = var_decl->getInit();
        if(init)
        {
            varDataNode->m_value = parseExpression(init, Context);
        }
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
    std::cout<<paddingDebugString;

    if(isa<CompoundStmt>(x))
    {
        std::cout<<"+";
        addNode(new COMPOUND_STMT_NODE(), Context, *x);
    }
    else if(isa<DeclStmt>(x))
    {
        std::cout<<"+";
        addNode(new DECL_STMT(), Context, *x);
    }
    else if(isa<BinaryOperator>(x))
    {
        std::cout<<"+";
        BinaryOperator* bin_op = dyn_cast<BinaryOperator>(x);
        addNode(new BINARY_OP(bin_op->getOpcode()), Context, *x);
    }
    else if(isa<DeclRefExpr>(x))
    {
        std::cout<<"+";
        auto decl_ref_expr = new DECL_REF_EXPR();
        addNode(decl_ref_expr, Context, *x);

        ValueDecl* decl = dyn_cast<DeclRefExpr>(x)->getDecl();

        if(decl)
        {
            NODE* var_decl_node = findDataNodeByASTNode(DynTypedNode::create(*decl));
            if(var_decl_node && isa<VAR_DECL>(var_decl_node))
            {
                decl_ref_expr->decl = dyn_cast<VAR_DECL>(var_decl_node);
                std::cout<<"[Stmt]: "<<x->getStmtClassName()<<std::endl;
                return true;
            }
        }
    }
    else if(isa<ImplicitCastExpr>(x))
    {
        std::cout<<"+";
        addNode(new IMPLICIT_CAST_EXPR(), Context, *x);
    }
    else if(isa<IntegerLiteral>(x))
    {
        std::cout<<"+";
        IntegerLiteral* int_literal = dyn_cast<IntegerLiteral>(x);
        Expr::EvalResult eval_result;
        int intValue = 0;
        if(int_literal->EvaluateAsInt(eval_result, *Context) == 1)
        {
            intValue = *eval_result.Val.getInt().getRawData();
        }
        addNode(new INT_LITERAL_EXPR(intValue), Context, *int_literal);
    }
    else if (isa<ReturnStmt>(x))
    {
        std::cout<<"+";

        RETURN_STMT* return_stmt_node = new RETURN_STMT();
        // EXPR_NODE* ret_expr_node = new EXPR_NODE();
        // dyn_cast<ReturnStmt>(x)->getRetValue()
        // return_stmt_node->return_expr = ret_expr_node;
        addNode(return_stmt_node, Context, *x);
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
    std::cout<<"call main: "<<mainFunctionNode<<std::endl;
    mainFunctionNode->call();
    // dumpTranslationUnit(&Context);
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
