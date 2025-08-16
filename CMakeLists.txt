cmake_minimum_required(VERSION 3.10)
project(AST_project)

set(LLVM_PROJECT_PATH /home/mykola/Repos/llvm-project)
set(CMAKE_LINKER_TYPE LLD)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ring main.cpp
    ASTCrawlerAction.h
        ASTCrawlerAction.cpp
	node.h
	node.cpp
	var_decl.h
	var_decl.cpp
	function_decl.h
	function_decl.cpp
	stmt_node.h
	expr_node.h
	compound_stmt_node.h
	compound_stmt_node.cpp
	decl_stmt.h
	decl_stmt.cpp
	decl_ref_expr.h
	decl_ref_expr.cpp
	binary_op.h
	binary_op.cpp
	return_stmt.h
	return_stmt.cpp
	int_literal_expr.h
	int_literal_expr.cpp
	implicit_cast_expr.h
	implicit_cast_expr.cpp
	expr_factory.h
	expr_factory.cpp
	executor.h
	executor.cpp
	scope.h
    )

target_compile_options(ring PUBLIC -fno-rtti -fno-exceptions)

target_include_directories(ring PUBLIC
	${LLVM_PROJECT_PATH}/clang/include
	${LLVM_PROJECT_PATH}/llvm/include
)

target_link_directories(ring PUBLIC
	/usr/local/lib
)

target_link_libraries(ring PUBLIC
	clangTooling
	clangFrontend
	clangParse
	clangDriver
	clangAPINotes
	clangSema
	clangSupport
	clangEdit
	clangAnalysis
	clangASTMatchers
	clangAST
	clangSerialization
	clangLex
	clangBasic
	LLVMWindowsDriver
	LLVMFrontendHLSL	
	LLVMMC
	LLVMMCParser
	LLVMFrontendOpenMP
	LLVMBitReader
	LLVMScalarOpts
	LLVMFrontendOffloading
	LLVMObjectYAML
	LLVMFrontendAtomic
	LLVMBinaryFormat
	LLVMDemangle
	LLVMTransformUtils
	LLVMAnalysis
	LLVMBitstreamReader
	LLVMProfileData
	LLVMDebugInfoDWARF
	LLVMObject
	LLVMTextAPI
	LLVMIRReader
	LLVMAsmParser
	LLVMCore
	LLVMRemarks
	LLVMTargetParser
	LLVMOption
	LLVMSupport
)

add_custom_target(other
    SOURCES file.cpp)
