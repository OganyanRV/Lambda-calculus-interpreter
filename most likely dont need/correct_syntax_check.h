// Copyright 2021 Oganyan Robert

#ifndef OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
#define OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include "terms_lib.h"

//using std::std::cout;
//using std::std::string;
//using std::vector;
//using std::to_string;

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

// Basically just transforms std::string to vector

std::vector<std::string> ParseToVec(std::string term) {
    std::vector<std::string> term_vector;
    LibFuncs library;
    for (size_t num = 0; num < term.size(); ++num) {
        if (term[num] == ' ') {
            continue;
        }
        if ((term[num] == ')') || (term[num] == '(')) {
            std::string element;
            element += term[num];
            term_vector.push_back(element);
            continue;
        }

        std::string element;
        element.push_back(term[num]);
        while ((num + 1 < term.size()) &&
               term[num + 1] != ' ' &&
               term[num + 1] != '(' &&
               term[num + 1] != ')') {
            element.push_back(term[num + 1]);
            ++num;
        }
        if ((element[0] >= 'A') && ((element[0] <= 'Z'))) {
            if (!library.exist("There is no such library function!"));
        }

        term_vector.push_back(element);
    }
    return term_vector;
}

//  Finds library functions in term (such as True, False, etc) and decompose them
bool ChangeLibFuncsToTerms(std::vector<std::string> &term_vec) {
    LibFuncs library;
    while (true) {
        bool need_to_parse = false;
        std::string new_term;
        for (size_t elem = 0; elem < term_vec.size(); elem++) {
            bool found_capital = false;
            std::string cur_term = term_vec[elem];
            for (char letter : cur_term) {
                if ((letter >= 'A') && (letter <= 'Z')) {
                    found_capital = true;
                    break;
                }
            }
            if (!found_capital) {
                new_term += cur_term;
                if (elem != term_vec.size() - 1) {
                    new_term += " ";
                }
                continue;
            }
            if (cur_term[0] == '\\') {
                if (!library.exist(cur_term.substr(1))) {
                    std::cout << "Invalid syntax: capital characters can be only used for library functions" << "\n";
                    return false;
                }
            } else {
                if (!library.exist(cur_term)) {
                    std::cout << "Invalid syntax: capital characters can be only used for library functions" << "\n";
                    return false;
                }
            }
            new_term += library[cur_term];
            if (elem != term_vec.size() - 1) {
                new_term += " ";
            }
            need_to_parse = true;
        }
        term_vec = ParseToVec(new_term);
        if (!need_to_parse) break;
    }
    return true;
}


#endif //OGANYAN_LAMBDA_CALC_CORRECT_SYNTAX_CHECK_H
