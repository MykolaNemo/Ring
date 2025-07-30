#ifndef ASTCRAWLERACTION_H
#define ASTCRAWLERACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"

using namespace clang;

class ASTCrawlerVisitor
    : public RecursiveASTVisitor<ASTCrawlerVisitor> {
public:
    explicit ASTCrawlerVisitor(ASTContext *Context);
    bool TraverseDecl(Decl *D);
    bool TraverseStmt(Stmt *x, DataRecursionQueue *Queue = nullptr);
    bool TraverseType(QualType x);
    // bool TraverseTypeLoc(TypeLoc TL);
    // bool TraverseAttr (Attr *At);
    // bool TraverseNestedNameSpecifier (NestedNameSpecifier *NNS);
    // bool TraverseNestedNameSpecifierLoc (NestedNameSpecifierLoc NNS);
    // bool TraverseDeclarationNameInfo (DeclarationNameInfo NameInfo);
    // bool TraverseTemplateName (TemplateName Template);
    // bool TraverseTemplateArgument (const TemplateArgument &Arg);
    // bool TraverseTemplateArgumentLoc (const TemplateArgumentLoc &ArgLoc);
    // bool TraverseTemplateArguments (ArrayRef< TemplateArgument > Args);
    // bool TraverseCXXBaseSpecifier (const CXXBaseSpecifier &Base);
    // bool TraverseConstructorInitializer (CXXCtorInitializer *Init);
    // bool TraverseLambdaCapture (LambdaExpr *LE, const LambdaCapture *C, Expr *Init);
    // bool TraverseSynOrSemInitListExpr (InitListExpr *S, DataRecursionQueue *Queue = nullptr);
    // bool TraverseObjCProtocolLoc (ObjCProtocolLoc ProtocolLoc);
    // bool TraverseConceptReference (ConceptReference *CR);

private:
    ASTContext *Context;
};

class ASTCrawlerConsumer : public clang::ASTConsumer {
public:
    explicit ASTCrawlerConsumer(ASTContext *Context);
    virtual void HandleTranslationUnit(clang::ASTContext &Context);

private:
    ASTCrawlerVisitor Visitor;
};

class ASTCrawlerAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile);
};

#endif // ASTCRAWLERACTION_H
