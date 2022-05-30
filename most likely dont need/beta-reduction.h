// Copyright 2021 Oganyan Robert

#ifndef OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
#define OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include "correct_syntax_check.h"
#include "../parse-syntax.h"


const int lambda = -1;
const int opening_bracket = -2;
const int closing_bracket = -3;
const int unprocessed = -4;

//  class for de_bruijn. index - distance, term - name, alpha_conversion_count - count of "'"
class Term {
public:
    int index;
    std::string term;
    int alpha_conversion_count;

    Term() {
        index = unprocessed;
        term = "";
        alpha_conversion_count = 0;
    }

    Term(int index_, std::string term_, int alpha_conversion_count_) :
            index(index_), term(std::move(term_)), alpha_conversion_count(alpha_conversion_count_) {}

    Term(int index_, std::string term_) :
            index(index_), term(std::move(term_)), alpha_conversion_count(0) {}

};

//  Making appropriate form of term for output in future

std::string GetOutPutString(const std::vector<Term> &term_vec_original) {
    auto term_vec = term_vec_original;
    std::string result_string_term;
    // Remove the most left and the most right brackets if they are useless
    while (true) {
        bool cycle_is_done = true;
        if ((term_vec[0].term == "(") && (term_vec[term_vec.size() - 1].term == ")")) {
            int brackets_count = 0;
            bool need_to_remove_brackets = true;
            for (size_t elem = 1; elem < term_vec.size() - 1; ++elem) {
                if (term_vec[elem].term == "(") {
                    brackets_count++;
                } else if (term_vec[elem].term == ")") {
                    brackets_count--;
                }
                if (brackets_count == -1) {
                    need_to_remove_brackets = false;
                    break;
                }
            }
            if (need_to_remove_brackets) {
                term_vec.pop_back();
                term_vec.erase(term_vec.begin());
                cycle_is_done = false;
            }
        }
        if (cycle_is_done) {
            break;
        }
    }

    // Need to rename back not necessary renamed variables
    std::map<std::string, std::set<int>> alpha_conversion_map;

    alpha_conversion_map["("].insert(0);
    alpha_conversion_map[")"].insert(0);

    // pre-processing
    for (const auto &term: term_vec) {
        if (term.term == "(" || term.term == ")") {
            continue;
        }
        if (term.term[0] == '\\') {
            alpha_conversion_map[term.term.substr(1)].insert(term.alpha_conversion_count);
        } else {
            alpha_conversion_map[term.term].insert(term.alpha_conversion_count);
        }
    }

    // making the result std::string considering correct amount of "'"
    for (size_t elem = 0; elem < term_vec.size(); ++elem) {
        auto cur_term = term_vec[elem];
        result_string_term += cur_term.term;
        std::string name_of_term;
        auto alpha_conversion_count = alpha_conversion_map["("].begin();
        if (cur_term.term[0] == '\\') {
            name_of_term = cur_term.term.substr(1);
            alpha_conversion_count = alpha_conversion_map[name_of_term].find(cur_term.alpha_conversion_count);
        } else {
            name_of_term = cur_term.term;
            alpha_conversion_count = alpha_conversion_map[name_of_term].find(cur_term.alpha_conversion_count);
        }

        while (true) {
            if (alpha_conversion_count == alpha_conversion_map[name_of_term].begin()) {
                break;
            }
            result_string_term += "'";
            --alpha_conversion_count;
        }

        if (elem + 1 < term_vec.size()) {
            auto next_term = term_vec[elem + 1];
            if ((cur_term.term != "(") && (next_term.term != ")")) {
                result_string_term += " ";
            }
        }

    }
    return result_string_term;
}


//  Calculates term with De Bruin Notation
std::vector<Term> ConvertToDeBruijnNotation(const std::vector<std::string> &term_vec) {

    std::vector<Term> term_de_bruijn(term_vec.size(), Term());
    std::map<std::string, int> usage_counter;

    // Main part of processing indexes and making alpha-conversion for NOT FREE (bounded) terms
    for (size_t cur_elem = 0; cur_elem < term_vec.size(); ++cur_elem) {
        if (term_de_bruijn[cur_elem].index != unprocessed) {
            continue;
        }
        std::string cur_term = term_vec[cur_elem];
        if (cur_term == "(") {
            term_de_bruijn[cur_elem] = Term(opening_bracket, cur_term);
            continue;
        }
        if (cur_term == ")") {
            term_de_bruijn[cur_elem] = Term(closing_bracket, cur_term);
            continue;
        }
        if (cur_term[0] == '\\') {
            term_de_bruijn[cur_elem] = Term(lambda, cur_term, usage_counter[cur_term.substr(1)]++);
            int brackets_in_lambda_count = 0;
            int end_lambda = -1;
            // true if opening bracket is for the following lambda ((\n n)); false if opening bracket is for application ((a b))
            std::stack<bool> brackets_type;
            for (size_t j = cur_elem + 1; j < term_vec.size(); ++j) {
                std::string cur_sub_term = term_vec[j];
                if (cur_sub_term == "(") {
                    if (term_vec[j + 1][0] == '\\') {
                        brackets_type.push(true);
                        brackets_in_lambda_count++;
                    } else {
                        brackets_type.push(false);
                    }
                    continue;
                } else if (cur_sub_term == ")") {
                    if (brackets_type.empty()) {
                        break;
                    }
                    if (brackets_type.top()) {
                        brackets_in_lambda_count--;
                    }
                    brackets_type.pop();
                    if (brackets_in_lambda_count == end_lambda - 1) {
                        end_lambda = -1;
                    }
                    continue;
                }
                if (end_lambda == -1 && cur_sub_term == cur_term.substr(1)) {
                    term_de_bruijn[j] = Term(brackets_in_lambda_count, cur_sub_term,
                                             term_de_bruijn[cur_elem].alpha_conversion_count);
                    continue;
                }
                if (cur_sub_term == cur_term) {
                    end_lambda = brackets_in_lambda_count;
                }
            }
        }
    }

    //  Pre-Processing for FREE terms.
    std::map<std::string, int> free_terms;
    for (int elem = static_cast<int>(term_vec.size()) - 1; elem >= 0; --elem) {
        if (term_de_bruijn[elem].index != unprocessed) {
            continue;
        }
        free_terms[term_vec[elem]] = free_terms.size() - 1;
    }

    int count_of_bound_terms = 0;
    std::stack<bool> brackets_type;
    //  Processing for FREE terms
    for (size_t elem = 0; elem < (int) term_vec.size(); ++elem) {
        if (term_vec[elem] == "(") {
            if (term_vec[elem + 1][0] == '\\') {
                count_of_bound_terms++;
                brackets_type.push(true);
            } else {
                brackets_type.push(false);
            }
        }
        if (term_vec[elem] == ")") {
            if (brackets_type.top()) {
                count_of_bound_terms--;
            }
            brackets_type.pop();
        }
        if (term_de_bruijn[elem].index == unprocessed) {
            term_de_bruijn[elem] = Term(free_terms[term_vec[elem]] + count_of_bound_terms, term_vec[elem],
                                        usage_counter[term_vec[elem]]);
        }
    }

    return term_de_bruijn;
}

//  Almost the same as previous one, but it includes alpha-conversion while processing

std::vector<Term> RecalculateDeBrujnNotation(const std::vector<Term> &term_vec) {
    std::vector<Term> term_de_bruijn(term_vec.size(), Term());
    std::map<std::string, int> usage_counter;

    // Main part of processing indexes and making alpha-conversion for NOT FREE (bounded) terms
    for (size_t cur_elem = 0; cur_elem < term_vec.size(); ++cur_elem) {
        if (term_de_bruijn[cur_elem].index != unprocessed) {
            continue;
        }
        std::string cur_term = term_vec[cur_elem].term;
        if (cur_term == "(") {
            term_de_bruijn[cur_elem] = Term(opening_bracket, cur_term);
            continue;
        }
        if (cur_term == ")") {
            term_de_bruijn[cur_elem] = Term(closing_bracket, cur_term);
            continue;
        }
        if (cur_term[0] == '\\') {
            term_de_bruijn[cur_elem] = Term(lambda, cur_term, usage_counter[cur_term.substr(1)]++);
            int brackets_in_lambda_count = 0;
            int end_lambda = -1;
            // true if opening bracket is for the following lambda ((\n n)); false if opening bracket is for application ((a b))
            std::stack<bool> brackets_type;
            for (size_t j = cur_elem + 1; j < term_vec.size(); ++j) {
                std::string cur_sub_term = term_vec[j].term;
                if (cur_sub_term == "(") {
                    if (term_vec[j + 1].term[0] == '\\') {
                        brackets_type.push(true);
                        brackets_in_lambda_count++;
                    } else {
                        brackets_type.push(false);
                    }
                    continue;
                } else if (cur_sub_term == ")") {
                    if (brackets_type.empty()) {
                        break;
                    }
                    if (brackets_type.top()) {
                        brackets_in_lambda_count--;
                    }
                    brackets_type.pop();
                    if (brackets_in_lambda_count == end_lambda - 1) {
                        end_lambda = -1;
                    }
                    continue;
                }
                if ((end_lambda == -1 && cur_sub_term == cur_term.substr(1))
                    && (term_vec[j].alpha_conversion_count == term_vec[cur_elem].alpha_conversion_count)) {
                    term_de_bruijn[j] = Term(brackets_in_lambda_count, cur_sub_term,
                                             term_de_bruijn[cur_elem].alpha_conversion_count);
                    continue;
                }
                if ((cur_sub_term == cur_term) &&
                    (term_vec[j].alpha_conversion_count == term_vec[cur_elem].alpha_conversion_count)) {
                    end_lambda = brackets_in_lambda_count;
                }
            }
        }
    }

    //  Pre-Processing for FREE terms
    std::map<std::string, int> free_terms;
    for (int elem = static_cast<int>(term_vec.size()) - 1; elem >= 0; --elem) {
        if (term_de_bruijn[elem].index != unprocessed) {
            continue;
        }
        free_terms[term_vec[elem].term] = free_terms.size() - 1;
    }

    int count_of_bound_terms = 0;
    std::stack<bool> brackets_type;

    //  Processing for FREE terms
    for (size_t elem = 0; elem < (int) term_vec.size(); ++elem) {
        if (term_vec[elem].term == "(") {
            if (term_vec[elem + 1].term[0] == '\\') {
                count_of_bound_terms++;
                brackets_type.push(true);
            } else {
                brackets_type.push(false);
            }
        }
        if (term_vec[elem].term == ")") {
            if (brackets_type.top()) {
                count_of_bound_terms--;
            }
            brackets_type.pop();
        }
        if (term_de_bruijn[elem].index == unprocessed) {
            term_de_bruijn[elem] = Term(free_terms[term_vec[elem].term] + count_of_bound_terms, term_vec[elem].term,
                                        usage_counter[term_vec[elem].term]);
        }
    }

    return term_de_bruijn;
}

//  Removes no need brackets for correct working of beta-reduction
std::vector<Term> RemoveUnnecessaryBrackets(const std::vector<Term> &term_vec) {
    auto return_vec = term_vec;

    //  Remove (), (\x) (x)
    while (true) {
        bool found_useless_brackets = false;
        for (size_t elem = 0; elem < return_vec.size(); ++elem) {
            if (return_vec[elem].term != "(") {
                continue;
            }
            if (return_vec[elem + 1].term == ")") {
                found_useless_brackets = true;
                return_vec.erase(return_vec.begin() + elem, return_vec.begin() + elem + 2);
                break;
            }
            if (return_vec[elem + 2].term == ")") {
                found_useless_brackets = true;
                return_vec.erase(return_vec.begin() + elem + 2);
                return_vec.erase(return_vec.begin() + elem);
                break;
            }
        }
        if (!found_useless_brackets) {
            break;
        }
    }


    // Remove ((*))
    while (true) {
        bool found_useless_brackets = false;
        for (size_t elem = 1; elem < return_vec.size(); ++elem) {
            if (return_vec[elem].term != "(" ||
                return_vec[elem - 1].term != "(") {
                continue;
            }
            int brackets_count = 0;
            for (size_t j = elem + 1; j < return_vec.size(); ++j) {
                if (return_vec[j].term == "(") {
                    brackets_count++;
                } else if (return_vec[j].term == ")") {
                    brackets_count--;
                    if (brackets_count == -1) {
                        if (return_vec[j + 1].term == ")") {
                            return_vec.erase(return_vec.begin() + j + 1);
                            return_vec.erase(return_vec.begin() + elem);
                            found_useless_brackets = true;
                        }
                        break;
                    }
                }
            }
        }
        if (!found_useless_brackets) {
            break;
        }
    }
    return return_vec;
}

// Reduce term in De Brujn's form with normal strategy (application for the outermost left redex)
std::vector<std::string> ReduceNormalStrat(const std::vector<Term> &term_vec_original) {
    auto term_vec = term_vec_original;
    std::vector<std::string> result_reduce;
    while (true) {
        std::vector<Term> current_reduce_step;

        term_vec = RemoveUnnecessaryBrackets(term_vec);
        result_reduce.push_back(GetOutPutString(term_vec));

        //  For some kind of terms (f.e. (\x x x)...) we need to recalculate DeBrujn indexes
        term_vec = RecalculateDeBrujnNotation(term_vec);

        //  If Beta-reduction is infinite and does not have normal form ((f.e Combinator Omega))
        if (result_reduce.size() >= 2) {
            if (result_reduce[result_reduce.size() - 1] == result_reduce[result_reduce.size() - 2]) {
                result_reduce.clear();
                result_reduce.emplace_back("Term has reached normal form");
                return result_reduce;
            }
        }


        bool term_changed = false;

        for (size_t elem = 0; elem < term_vec.size(); ++elem) {
            if (term_vec[elem].index != lambda) {
                continue;
            }
            int brackets_count = 0;
            //  Getting the expression into which we will perform the substitution
            int lambda_end_bracket = static_cast<int>(term_vec.size()) - 1;
            for (size_t j = elem + 1; j < term_vec.size(); ++j) {
                if (term_vec[j].index == opening_bracket) {
                    brackets_count++;
                } else if (term_vec[j].index == closing_bracket) {
                    brackets_count--;
                    if (brackets_count == -1) {
                        lambda_end_bracket = j;
                        break;
                    }
                }
            }

            //  Found no substituted expression
            if (lambda_end_bracket == term_vec.size() - 1) {
                continue;
            }

            // Found one
            std::vector<Term> substituted_term;
            if (term_vec[lambda_end_bracket + 1].index >= 0) {
                substituted_term.push_back(term_vec[lambda_end_bracket + 1]);
            } else if (term_vec[lambda_end_bracket + 1].index == opening_bracket) {
                brackets_count = 0;
                substituted_term.push_back(term_vec[lambda_end_bracket + 1]);
                for (int j = lambda_end_bracket + 2; j < (int) term_vec.size(); j++) {
                    if (term_vec[j].index == opening_bracket) {
                        brackets_count++;
                    } else if (term_vec[j].index == closing_bracket) {
                        brackets_count--;
                    }
                    substituted_term.push_back(term_vec[j]);
                    if (brackets_count == lambda) {
                        break;
                    }
                }
            }

            if (substituted_term.empty()) {
                continue;
            }

            for (size_t elem_before_application = 0; elem_before_application < elem - 1; ++elem_before_application) {
                current_reduce_step.push_back(term_vec[elem_before_application]);
            }

            //  True if lambda stays after opening bracket
            std::stack<bool> bracket_type;
            brackets_count = 0; //  Count of lambdas in current scope

            //  The main part. The one step of beta-reduction is processing here.
            //  I don't know language that well to describe everything whats going here
            for (size_t j = elem + 1; j < lambda_end_bracket; j++) {
                if (term_vec[j].index == opening_bracket) {
                    if (term_vec[j + 1].index == lambda) {
                        bracket_type.push(true);
                        brackets_count++;
                    } else {
                        bracket_type.push(false);
                    }
                }
                if (term_vec[j].index == closing_bracket) {
                    if (bracket_type.top()) {
                        brackets_count--;
                    }
                    bracket_type.pop();
                }
                if (term_vec[j].index < brackets_count) {
                    current_reduce_step.push_back(term_vec[j]);
                } else if (term_vec[j].index > brackets_count) {
                    current_reduce_step.emplace_back(term_vec[j].index - 1, term_vec[j].term,
                                                     term_vec[j].alpha_conversion_count);
                } else {
                    std::stack<bool> substituted_term_bracket_type;
                    int item_cnt = 0;
                    for (size_t k = 0; k < substituted_term.size(); ++k) {
                        if (substituted_term[k].index == opening_bracket) {
                            if (substituted_term[k + 1].index == lambda) {
                                substituted_term_bracket_type.push(true);
                                item_cnt++;
                            } else {
                                substituted_term_bracket_type.push(false);
                            }
                        }
                        if (substituted_term[k].index == closing_bracket) {
                            if (substituted_term_bracket_type.top()) {
                                item_cnt--;
                            }
                            substituted_term_bracket_type.pop();
                        }
                        if (item_cnt <= substituted_term[k].index) {
                            current_reduce_step.emplace_back(substituted_term[k].index + brackets_count,
                                                             substituted_term[k].term,
                                                             substituted_term[k].alpha_conversion_count);
                        } else {
                            current_reduce_step.push_back(substituted_term[k]);
                        }
                    }
                }
            }
            for (size_t elem_after_application = lambda_end_bracket + substituted_term.size() + 1;
                 elem_after_application < term_vec.size(); elem_after_application++) {
                current_reduce_step.push_back(term_vec[elem_after_application]);
            }
            term_changed = true;
            break;
        }
        if (!term_changed) {
            break;
        }
        term_vec = current_reduce_step;
    }
    return result_reduce;
}


std::vector<std::string> BetaReduction(const std::string &term) {
    //  Check for correct input
    if (!IsSyntaxCorrect(term)) {
        return std::vector<std::string>();
    }

    //  Make appropriate form of term
    auto good_form_term = MakeCorrectForm(term);
    std::cout << good_form_term << std::endl;
    return {};
//    if (!IsSyntaxCorrect(good_form_term)) {
//        return std::vector<std::string>();
//    }
//
//    //  Convert std::string to std::vector
//    std::vector<std::string> term_vec = ParseToVec(good_form_term);
//
//    //  Check for correct syntax again
//
//    if (!IsSyntaxCorrect(term_vec)) {
//        return std::vector<std::string>();
//    }
//
//    //  Find Functions from Library and decompose them
//
//    if (!ChangeLibFuncsToTerms(term_vec)) {
//        return std::vector<std::string>();
//    }
//
//    //  Calculate De Brujn Notation
//    std::vector<Term> term_de_bruijn = ConvertToDeBruijnNotation(term_vec);
//
//#ifdef DEBUG
//
//    cout << "\n";
//
//    for (const auto& elem: term_de_bruijn) {
//        cout << elem.term << " ";
//    }
//    cout << "\n";
//
//    for (const auto& elem: term_de_bruijn) {
//        cout << elem.index << " ";
//    }
//    cout << "\n";
//
//    for (const auto& elem: term_de_bruijn) {
//        cout << elem.alpha_conversion_count << " ";
//    }
//    cout << "\n";
//#endif
//
//    //  Beta-reduction
//
//    std::vector<std::string> reduction_result = ReduceNormalStrat(term_de_bruijn);
//    return reduction_result;
}

#endif //OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
