#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include "ASTCrawlerAction.h"

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static cl::OptionCategory MyToolCategory("Ring options");

int main(int argc, const char **argv) {
    // CommonOptionsParser::create will parse arguments and create a
    // CompilationDatabase.
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
        // Fail gracefully for unsupported options.
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser& OptionsParser = ExpectedParser.get();

    // Use OptionsParser.getCompilations() and OptionsParser.getSourcePathList()
    // to retrieve CompilationDatabase and the list of input file paths.
    // for(const auto& sourceFile : OptionsParser.getCompilations().getAllFiles())
    // {
    //     std::cout<<sourceFile<<"\n";
    // }
    for(const auto& sourceFile : OptionsParser.getSourcePathList())
    {
        std::cout<<sourceFile<<"\n";
    }
    // for(const auto& compileCommand : OptionsParser.getCompilations().getAllCompileCommands())
    // {
    //     for(const auto& line : compileCommand.CommandLine)
    //     {
    //         std::cout<<line<<" ";
    //     }
    //     std::cout<<"\n";
    // }
    // std::cout.flush();

    // ASTUnit
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());
    // int result = Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
    int result = Tool.run(newFrontendActionFactory<ASTCrawlerAction>().get());
    // return clang::tooling::runToolOnCode(std::make_unique<ASTCrawlerAction>(), "int main(){}");
    return result;
}
