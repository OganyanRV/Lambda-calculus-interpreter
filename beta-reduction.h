#ifndef OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
#define OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
#include <vector>
#include <string>
#include <iostream>
#include "parse-syntax.h"
#include "correct-syntax-check.h"
#include "include/AbstractSyntaxTree.h"
std::vector<std::string> BetaReduction(const std::string &term) {
//    //  Check for correct input
//    if (!IsSyntaxCorrect(term)) {
//        return std::vector<std::string>();
//    }
//
//    //  Make appropriate form of term
//    auto correct_form_term = MakeCorrectForm(term);
////    std::cout << correct_form_term << std::endl;
////    correct_form_term = RemoveSpaces(correct_form_term);
////    std::cout << correct_form_term << std::endl;
//
//    AbstractSyntaxTree tree(correct_form_term);
////
////    std::cout<<tree.ExprToString(tree.GetRoot())<< std::endl;
////    std::cout<<tree.ExprToStringDB(tree.GetRoot())<< std::endl;
//// Test
//
////    auto lmao = tree.CopySubTree(tree.GetRoot());
////    tree.MakeReductionStep(tree.GetRoot());
////    std::cout<<tree.ExprToStringDB(tree.GetRoot())<< std::endl;
//    auto tree1 = tree;
//    std::cout<<"Applicative strat: "<<std::endl;
//    auto eager = tree1.CallByValueReduction();
//    for (auto& step : eager) {
//        std::cout<<step<<std::endl;
//    }
//
//    auto tree2 = tree;
//    std::cout<<"Lazy strat: "<<std::endl;
//    auto lazy = tree2.CallByNameReduction();
//    for (auto& step : lazy) {
//        std::cout<<step<<std::endl;
//    }
//
//    auto tree3 = tree;
//    std::cout<<"Normal strat: "<<std::endl;
//    auto norm = tree3.NormalReduction();
//    for (auto& step : norm) {
//        std::cout<<step<<std::endl;
//    }
//    int kek = 0;
//    return {};
}
#endif//OGANYAN_LAMBDA_CALC_BETA_REDUCTION_H
