#ifndef OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
#define OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
#include <vector>
#include <string>
#include <iostream>

//  Check if term is correctly written. If not, returns false and prints the mistake.

bool IsSyntaxCorrect(const std::string &term) {

    // Check for correct brackets

    int brackets_count = 0;
    for (const char &elem : term) {
        if (elem == '(') {
            brackets_count++;
        } else if (elem == ')') {
            brackets_count--;
        }
        if (brackets_count < 0) {
            std::cout << "Invalid syntax: some ending brackets dont belong to opening brackets" << "\n";
            return false;
        }
    }

    if (brackets_count < 0) {
        std::cout << "Invalid syntax: invalid syntax: too much of ending brackets" << "\n";
        return false;
    }

    if (brackets_count > 0) {
        std::cout << "Invalid syntax: invalid syntax: too much of opening brackets" << "\n";
        return false;
    }

    //  Check for existing lambdas

    bool exist_lib_fun = false;
    for (char num : term) {
        if ((num >= 'A' && num <= 'Z')) {
            exist_lib_fun = true;
            break;
        }
    }
    if (!exist_lib_fun) {
        for (size_t num = 0; num < term.size(); ++num) {
            if (term[num] == '\\') {
                break;
            } else {
                if (num == term.size() - 1) {
                    std::cout << "There are no lambdas: nothing to reduce" << "\n";
                    return false;
                }
            }

        }
    }


    //  Check for correct lambda-usages

    for (size_t num = 0; num < term.size() - 1; ++num) {
        if (term[num] == '\\' && !isalpha(term[num + 1])) {
            std::cout << "Invalid syntax: there is no variable after lambda" << "\n";
            return false;
        }
    }

    if (term[term.size() - 1] == '\\') {
        std::cout << "Invalid syntax: there is no variable after lambda" << "\n";
        return false;
    }

    return true;
}

bool IsSyntaxCorrect(const std::vector<std::string> &term_vec) {
    for (size_t elem = 0; elem < term_vec.size() - 1; ++elem) {
        if (term_vec[elem][0] == '\\') {
            bool is_correct = false;
            for (size_t j = elem + 1; j < term_vec.size(); ++j) {
                if (term_vec[j] != ")") {
                    is_correct = true;
                }
            }
            if (!is_correct) {
                std::cout << "Invalid syntax: there is no variable after lambda" << "\n";
                return false;
            }
        }
    }
    return true;
}
#endif//OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
