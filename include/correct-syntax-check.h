#ifndef OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
#define OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
#include "Enums.h"
#include <iostream>
#include <string>
#include <vector>

//  Check if term is correctly written. If not, returns false and prints the mistake. It covers SOME mistakes.
bool IsSyntaxCorrect(const std::string &term, InputType input_type) {

    // Check for correct brackets
    int brackets_count = 0;
    for (auto elem : term) {
        if (elem == '(') {
            ++brackets_count;
        } else if (elem == ')') {
            --brackets_count;
        }
        if (brackets_count < 0) {
            std::cout << "Invalid syntax: some ending brackets dont belong to opening brackets"
                      << "\n";
            return false;
        }
    }
    if (brackets_count < 0) {
        std::cout << "Invalid syntax: invalid syntax: too much of ending brackets"
                  << "\n";
        return false;
    }
    if (brackets_count > 0) {
        std::cout << "Invalid syntax: invalid syntax: too much of opening brackets"
                  << "\n";
        return false;
    }

    //  Check for existing lambdas
    if (input_type == InputType::kNormal) {
        bool exist_lib_fun = false;
        for (auto elem : term) {
            if ((elem >= 'A' && elem <= 'Z')) {
                exist_lib_fun = true;
                break;
            }
        }
        if (!exist_lib_fun) {
            if (term.find("\\") == term.npos) {
                std::cout << "There are no lambdas: nothing to reduce"
                          << "\n";
                return false;
            }
        }
    } else if (input_type == InputType::kHaskell) {
        if (term.find("Abs") == term.npos) {
            std::cout << "There are no lambdas: nothing to reduce"
                      << "\n";
            return false;
        }
    } else if (input_type == InputType::kDeBruijn) {
        if (term.find("\\") == term.npos) {
            std::cout << "There are no lambdas: nothing to reduce"
                      << "\n";
            return false;
        }
    }

    //  Check for correct lambda-usages
    if (input_type == InputType::kNormal) {
        for (size_t idx = 0; idx < term.size() - 1; ++idx) {
            if (term[idx] == '\\' && !isalpha(term[idx + 1])) {
                std::cout << "Invalid syntax: there is no variable after lambda"
                          << "\n";
                return false;
            }
        }
        if (term[term.size() - 1] == '\\') {
            std::cout << "Invalid syntax: there is no variable after lambda"
                      << "\n";
            return false;
        }
    }

    return true;
}

#endif//OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
