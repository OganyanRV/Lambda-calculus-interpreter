#ifndef OGANYAN_LAMBDA_CALC_SYNTAX_FUNCTIONS_H
#define OGANYAN_LAMBDA_CALC_SYNTAX_FUNCTIONS_H
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


std::string PutBracketsAroundLambdaAbstractions(const std::string &term) {
    std::string corrected_term{term};
    while (true) {
        std::string new_term;
        bool changed = false;
        for (size_t pos_for_opening_bracket = 0;
             pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
            if (
                    (corrected_term[pos_for_opening_bracket] == '\\' && (!pos_for_opening_bracket)) ||
                    (corrected_term[pos_for_opening_bracket] == '\\' &&
                     (corrected_term[pos_for_opening_bracket - 1] != '('))) {
                int brackets_count = 0;
                int pos_for_ending_bracket = corrected_term.size();
                for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                    if (corrected_term[j] == '(') {
                        ++brackets_count;
                    } else if (corrected_term[j] == ')') {
                        --brackets_count;
                    }
                    if (brackets_count < 0) {
                        pos_for_ending_bracket = j;
                        break;
                    }
                }
                new_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                           corrected_term.substr(pos_for_opening_bracket,
                                                 pos_for_ending_bracket - pos_for_opening_bracket) +
                           ')' +
                           corrected_term.substr(pos_for_ending_bracket,
                                                 corrected_term.size() - pos_for_ending_bracket);
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
        corrected_term = new_term;
    }
    return corrected_term;
}

std::string PutBracketsAroundSubTermsInLambdaAbstractions(const std::string &term) {
    std::string corrected_term{term};
    while (true) {
        std::string new_term;
        bool changed = false;

        for (size_t pos_for_opening_bracket = 0;
             pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
            if (corrected_term[pos_for_opening_bracket] == '\\' &&
                corrected_term[pos_for_opening_bracket + 3] != '(') {
                int brackets_count = 0;
                int pos_for_ending_bracket = corrected_term.size();
                for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                    if (corrected_term[j] == '(') {
                        ++brackets_count;
                    } else if (corrected_term[j] == ')') {
                        --brackets_count;
                    }
                    if (brackets_count < 0) {
                        pos_for_ending_bracket = j;
                        break;
                    }
                }
                new_term = corrected_term.substr(0, pos_for_opening_bracket + 3) + '(' +
                           corrected_term.substr(pos_for_opening_bracket + 3,
                                                 pos_for_ending_bracket - pos_for_opening_bracket - 3) +
                           ')' +
                           corrected_term.substr(pos_for_ending_bracket,
                                                 corrected_term.size() - pos_for_ending_bracket);
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
        corrected_term = new_term;
    }
    return corrected_term;
}

std::string PutBracketsInApplications(const std::string &term) {
    std::string corrected_term{term};

    size_t count_of_inserted_brackets_pairs = 0;
    for (size_t pos_for_opening_bracket = 0;
         pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
        count_of_inserted_brackets_pairs = 0;
        if (corrected_term[pos_for_opening_bracket] == '(') {
            int brackets_count = 0;
            int pos_for_ending_bracket = corrected_term.size();
            for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                if (corrected_term[j] == '(') {
                    ++brackets_count;
                } else if (corrected_term[j] == ')') {
                    --brackets_count;
                }
                if (brackets_count < 0) {
                    pos_for_ending_bracket = j;
                    break;
                }
            }

            for (size_t j = pos_for_ending_bracket + 1; j < corrected_term.size(); ++j) {
                if (corrected_term[j] == ' ') {
                    continue;
                }

                if (corrected_term[j] == ')') {
                    break;
                }

                // (\x x) a => ((\x x) a)
                if (std::isalpha(corrected_term[j])) {
                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket + 1) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket + 1,
                                                           j - pos_for_opening_bracket) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }

                // (\x x) (\y y) a => ((\x x) (\y y)) a
                if (corrected_term[j] == '(') {
                    brackets_count = 0;
                    ++j;
                    while (j < corrected_term.size()) {
                        if (corrected_term[j] == '(') {
                            ++brackets_count;
                        } else if (corrected_term[j] == ')') {
                            --brackets_count;
                        }
                        if (brackets_count < 0) {
                            pos_for_ending_bracket = j;
                            break;
                        }
                        ++j;
                    }

                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket + 1) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket + 1,
                                                           pos_for_ending_bracket - pos_for_opening_bracket) +
                                     ')' +
                                     corrected_term.substr(pos_for_ending_bracket + 1,
                                                           corrected_term.size() - pos_for_ending_bracket - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }
            }

            pos_for_opening_bracket += count_of_inserted_brackets_pairs;
        }

        else if (std::isalpha(corrected_term[pos_for_opening_bracket]) &&
                 (!pos_for_opening_bracket ||
                  (pos_for_opening_bracket && corrected_term[pos_for_opening_bracket - 1] != '\\'))) {
            bool done = false;
            for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size() && !done; ++j) {
                if (corrected_term[j] == ')') {
                    done = true;
                }

                if (std::isalpha(corrected_term[j])) {
                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket,
                                                           j - pos_for_opening_bracket + 1) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }

                if (corrected_term[j] == '(') {
                    int brackets_count = 0;
                    ++j;
                    while (j < corrected_term.size()) {
                        if (corrected_term[j] == '(') {
                            ++brackets_count;
                        } else if (corrected_term[j] == ')') {
                            --brackets_count;
                        }
                        if (brackets_count < 0) {
                            break;
                        }
                        ++j;
                    }

                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket,
                                                           j - pos_for_opening_bracket + 1) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }
            }
            pos_for_opening_bracket += count_of_inserted_brackets_pairs;
        }
    }
    return corrected_term;
}


//  Makes appropriate form of expression
std::string MakeCorrectForm(const std::string &term) {
    std::string corrected_term{term};
    corrected_term = PutBracketsAroundLambdaAbstractions(corrected_term);
    corrected_term = PutBracketsAroundSubTermsInLambdaAbstractions(corrected_term);
    corrected_term = PutBracketsInApplications(corrected_term);
    return corrected_term;
}

#endif//OGANYAN_LAMBDA_CALC_SYNTAX_FUNCTIONS_H
