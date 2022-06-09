cmake_minimum_required(VERSION 3.17)
project(oganyan_lambda_calc)

set(CMAKE_CXX_STANDARD 14)

add_executable(Interpreter main.cpp include/AbstractSyntaxTree.h include/Enums.h include/TermsLib.h
        include/TermGenerator.h include/TermNode.h src/TermGenerator.cpp src/AbstractSyntaxTree.cpp include/Syntax_functions.h)

add_executable(Experiments experiment/test.cpp include/AbstractSyntaxTree.h include/Enums.h
        include/TermGenerator.h include/TermNode.h src/TermGenerator.cpp src/AbstractSyntaxTree.cpp include/Syntax_functions.h)