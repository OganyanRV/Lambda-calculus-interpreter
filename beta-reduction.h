#ifndef OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
#define OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
#include <vector>
#include <string>
#include <iostream>
#include "parse-syntax.h"
#include "correct-syntax-check.h"
#include "AbstractSyntaxTree.h"
std::vector<std::string> BetaReduction(const std::string &term) {
    //  Check for correct input
    if (!IsSyntaxCorrect(term)) {
        return std::vector<std::string>();
    }

    //  Make appropriate form of term
    auto correct_form_term = MakeCorrectForm(term);
    std::cout << correct_form_term << std::endl;
//    correct_form_term = RemoveSpaces(correct_form_term);
//    std::cout << correct_form_term << std::endl;

    AbstractSyntaxTree tree(correct_form_term);
    int kek = 0;
    return {};
}
#endif//OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
