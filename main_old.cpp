// #include <clang-c/Index.h>
#include <clang/AST/ASTContext.h>
#include <iostream>
#include <vector>

// struct VarDefinition;

// struct TypeDefinition
// {
//     std::string name;
//     bool primitive = true;
//     std::vector<VarDefinition> fields;
// };

struct VarDefinition
{
    std::string name;
    std::string type;
};

struct FunctionDecl
{
    std::string name;
    std::string returnType;
    std::vector<VarDefinition> parameters;
    bool hasBody = false;
};

std::vector<FunctionDecl> allFunctions;

FunctionDecl* currentFunction = nullptr;

struct ClientData
{
    CXCursorKind kind;
    void* data = nullptr;
    ClientData* parentData = nullptr;
};

ClientData* toClientData(CXClientData client_data)
{
    return reinterpret_cast<ClientData*>(client_data);
}

// // std::vector<VarDefinition> allVariables;
// CXChildVisitResult visitNode(CXCursor current_cursor, CXCursor parent, CXClientData client_data);

// void visitStructType(const CXCursor& cursor)
// {
//     TypeDefinition structType;
//     structType.primitive = false;

//     if(!clang_Cursor_isAnonymous(cursor))
//     {
//         auto struct_name = clang_getCursorDisplayName(cursor);
//         auto c_struct_name = clang_getCString(struct_name);
//         structType.name = c_struct_name;
//         clang_disposeString(struct_name);
//     }
//     else
//     {
//         structType.name = "unnamed_struct";
//     }
//     clang_visitChildren(cursor, &visitNode, nullptr);

//     allTypes.push_back(structType);
//     // std::cout<<"struct "<<structType.name<<std::endl;
// }

// void addUnionType(const CXCursor& cursor)
// {
//     TypeDefinition unionType;
//     unionType.primitive = false;

//     if(!clang_Cursor_isAnonymous(cursor))
//     {
//         auto union_name = clang_getCursorDisplayName(cursor);
//         auto c_union_name = clang_getCString(union_name);
//         unionType.name = c_union_name;
//         clang_disposeString(union_name);
//     }
//     else
//     {
//         unionType.name = "unnamed_union";
//     }

//     allTypes.push_back(unionType);
//     // std::cout<<"struct "<<structType.name<<std::endl;
// }

// void addClassType(const CXCursor& cursor)
// {
//     TypeDefinition classType;
//     classType.primitive = false;

//     if(!clang_Cursor_isAnonymous(cursor))
//     {
//         auto class_name = clang_getCursorDisplayName(cursor);
//         auto c_class_name = clang_getCString(class_name);
//         classType.name = c_class_name;
//         clang_disposeString(class_name);
//     }
//     else
//     {
//         classType.name = "unnamed_class";
//     }

//     allTypes.push_back(classType);
// }

// void addEnumType(const CXCursor& cursor)
// {
//     TypeDefinition enumType;
//     enumType.primitive = false;

//     if(!clang_Cursor_isAnonymous(cursor))
//     {
//         auto enum_name = clang_getCursorDisplayName(cursor);
//         auto c_enum_name = clang_getCString(enum_name);
//         enumType.name = c_enum_name;
//         clang_disposeString(enum_name);
//     }
//     else
//     {
//         enumType.name = "unnamed_enum";
//     }

//     allTypes.push_back(enumType);
// }

// void addField(const CXCursor& current_cursor, const CXCursor& parent_cursor)
// {

// }

bool visitFunctionBody(CXCursor current_cursor)
{
    static const auto l = [](CXCursor cur, CXCursor, CXClientData)->CXChildVisitResult{
        if(clang_getCursorKind(cur) == CXCursor_CompoundStmt)
        {
            return CXChildVisit_Break;
        }
        return CXChildVisit_Continue;
    };

    auto result = clang_visitChildren(current_cursor, l, nullptr);
    return (result != 0);
}

bool functionHasBody(CXCursor current_cursor)
{
    static const auto searchCompoundStmt = [](CXCursor cur, CXCursor, CXClientData)->CXChildVisitResult{
        if(clang_getCursorKind(cur) == CXCursor_CompoundStmt)
        {
            return CXChildVisit_Break;
        }
        return CXChildVisit_Continue;
    };

    auto result = clang_visitChildren(current_cursor, searchCompoundStmt, nullptr);
    return (result != 0);
}

void evaluate(CXCursor cur)
{
    void* evalResult = clang_Cursor_Evaluate(cur);
    std::cout<<"eval kind: "<<clang_EvalResult_getKind(evalResult)<<std::endl;
    // if(clang_EvalResult_getKind(evalResult) == CXEval_Int)
    // {
        // std::cout<<"Integer: "<<clang_EvalResult_getAsInt(evalResult)<<std::endl;
    // }
    clang_EvalResult_dispose(evalResult);
}

CXChildVisitResult visitNode(CXCursor current_cursor, CXCursor parent, CXClientData client_data)
{
    const auto kind = clang_getCursorKind(current_cursor);
    switch(kind)
    {
    // case CXCursor_StructDecl:
    // {
    //     visitStructType(current_cursor);
    //     break;
    // }
    // case CXCursor_UnionDecl:
    // {
    //     addUnionType(current_cursor);
    //     break;
    // }
    // case CXCursor_ClassDecl:
    // {
    //     addClassType(current_cursor);
    //     break;
    // }
    // case CXCursor_EnumDecl:
    // {
    //     addEnumType(current_cursor);
    //     break;
    // }
    // case CXCursor_FieldDecl:
    // {
    //     addField(current_cursor, parent);
    //     break;
    // }
    case CXCursor_FunctionDecl:
    {
        FunctionDecl function;

        // Function name
        CXString func_name = clang_getCursorSpelling(current_cursor);
        function.name = clang_getCString(func_name);
        clang_disposeString(func_name);

        // Function return type
        CXString ret_type = clang_getTypeSpelling(clang_getCursorResultType(current_cursor));
        function.returnType = clang_getCString(ret_type);
        clang_disposeString(ret_type);

        // Function arguments
        unsigned int numArg = clang_Cursor_getNumArguments(current_cursor);
        for(unsigned int i = 0; i < numArg; ++i)
        {
            VarDefinition var;
            CXCursor argCursor = clang_Cursor_getArgument(current_cursor, i);

            CXString arg_name = clang_getCursorSpelling(argCursor);
            var.name = clang_getCString(arg_name);
            clang_disposeString(arg_name);

            CXString arg_type = clang_getTypeSpelling(clang_getCursorType(argCursor));
            var.type = clang_getCString(arg_type);
            clang_disposeString(arg_type);
            function.parameters.push_back(var);
        }

        function.hasBody = clang_isCursorDefinition(current_cursor);

        allFunctions.push_back(function);
        if(function.hasBody)
        {
            ClientData data;
            data.kind = CXCursor_FunctionDecl;
            data.data = &allFunctions.back();
            clang_visitChildren(current_cursor, &visitNode, &data);
        }
        break;
    }
    case CXCursor_ParmDecl:
    {
        //ignore
        break;
    }
    case CXCursor_CompoundStmt:
    {
        if((clang_getCursorKind(parent) == CXCursor_FunctionDecl) && client_data)
        {
            ClientData* data = toClientData(client_data);
            if(data->kind == CXCursor_FunctionDecl) // function body
            {
                std::cout<<"Body of a function: "<<reinterpret_cast<FunctionDecl*>(data->data)->name<<std::endl;
                ClientData compoundData;
                compoundData.kind = CXCursor_CompoundStmt;
                compoundData.parentData = data;
                clang_visitChildren(current_cursor, &visitNode, &compoundData);
            }
        }
        break;
    }
    case CXCursor_VarDecl:
    {
        CXString str = clang_getCursorSpelling(current_cursor);
        std::cout<<"VarDecl: "<<clang_getCString(str)<<std::endl;

        void* evalResult = clang_Cursor_Evaluate(current_cursor);
        // std::cout<<"VarDecl eval kind: "<<clang_EvalResult_getKind(evalResult)<<std::endl;
        if(clang_EvalResult_getKind(evalResult) == CXEval_Int)
        {
            std::cout<<clang_getCString(str)<<" = "<<clang_EvalResult_getAsInt(evalResult)<<std::endl;
        }
        else
        {
            evaluate(current_cursor);
        }
        clang_EvalResult_dispose(evalResult);
        clang_disposeString(str);

        // clang_visitChildren(current_cursor, &visitNode, nullptr);
        break;
    }
    case CXCursor_IntegerLiteral:
    {
        CXString str = clang_getCursorSpelling(current_cursor);
        void* evalResult = clang_Cursor_Evaluate(current_cursor);
        if(clang_EvalResult_getKind(evalResult) == CXEval_Int)
        {
            std::cout<<"Integer: "<<clang_EvalResult_getAsInt(evalResult)<<std::endl;
        }
        clang_EvalResult_dispose(evalResult);
        clang_disposeString(str);
        clang_visitChildren(current_cursor, &visitNode, nullptr);
        break;
    }
    case CXCursor_BinaryOperator:
    {
        CXString str = clang_getCursorSpelling(current_cursor);
        std::cout<<"BinaryOperator: "<<clang_getCString(str)<<std::endl;
        clang_disposeString(str);

        evaluate(current_cursor);
        clang_visitChildren(current_cursor, &visitNode, nullptr);
        break;
    }
    case CXCursor_DeclStmt:
    {
        // CXString str = clang_getCursorSpelling(current_cursor);
        // std::cout<<"DeclStmt: "<<clang_getCString(str)<<std::endl;
        // void* evalResult = clang_Cursor_Evaluate(current_cursor);
        // auto evalKind = clang_EvalResult_getKind(evalResult);
        // std::cout<<"DeclStmt eval kind: "<<evalKind<<std::endl;
        // clang_EvalResult_dispose(evalResult);
        // clang_disposeString(str);
        clang_visitChildren(current_cursor, &visitNode, nullptr);
        break;
    }
    case CXCursor_DeclRefExpr:
    {
        CXString str = clang_getCursorSpelling(current_cursor);
        std::cout<<"CXCursor_DeclRefExpr: "<<clang_getCString(str)<<std::endl;
        clang_disposeString(str);
                evaluate(current_cursor);
        clang_visitChildren(current_cursor, &visitNode, nullptr);
        break;
    }
    default:
        std::cout<<"Unknown cursor kind: "<<kind<<"\n-------------------------\n";
        CXString str = clang_getCursorSpelling(current_cursor);
                evaluate(current_cursor);
        std::cout<<clang_getCString(str)<<"\n-------------------------"<<std::endl;
        clang_disposeString(str);
        break;
    }
    return CXChildVisit_Continue;
}

int main(){
    CXIndex index = clang_createIndex(0, 0); //Create index
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        "file.cpp", nullptr, 0,
        nullptr, 0,
        CXTranslationUnit_None); //Parse "file.cpp"


    if (unit == nullptr){
        std::cerr << "Unable to parse translation unit. Quitting.\n";
        return 0;
    }
    //Obtain a cursor at the root of the translation unit
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, &visitNode, nullptr);

    std::cout<<"\nAll types:\n--------------------------------\n";
    for(const auto& func : allFunctions)
    {
        std::cout<<func.returnType<<" "<<func.name<<"(";
        for(size_t i = 0; i < func.parameters.size(); ++i)
        {
            std::cout<<func.parameters[i].type<<" "<<func.parameters[i].name;
            if(i < func.parameters.size()-1)
            {
                std::cout<<", ";
            }
        }
        std::cout<<")";
        if(func.hasBody)
        {
            std::cout<<" {}"<<std::endl;
        }
        else
        {
            std::cout<<";"<<std::endl;
        }
    }
}