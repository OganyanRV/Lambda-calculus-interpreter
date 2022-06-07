cmake_minimum_required(VERSION 3.17)
project(oganyan_lambda_calc)

set(CMAKE_CXX_STANDARD 14)
include_directories(oganyan_lambda_calc include)
#add_executable(oganyan_lambda_calc "most likely dont need/main.cpp" "most likely dont need/correct_syntax_check.h" "most likely dont need/beta-reduction.h" "most likely dont need/terms_lib.h")
add_executable(lambda_calculus main.cpp include/AbstractSyntaxTree.h include/TermGenerator.h include/TermNode.h src/TermGenerator.cpp)