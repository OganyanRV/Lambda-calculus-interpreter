#include "../include/TermGenerator.h"
#include <cassert>
#include <iostream>
#include <string>

TermGenerator terms_generator;

void AverageReductionChainLength() {
    std::vector<double> average_lengths;
    std::vector<std::string> no_normal_form_terms;
    size_t max_size_of_term = 7;
    for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
        std::vector<int64_t> cur_lengths;

        for (int64_t term_num = terms_generator.GetCount(term_size - 1, 1);
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            std::cout << term_size << " " << 0 << " " << term_num << std::endl;
            //            auto str = terms_generator.GenerateTermStr(term_size, 0, term_num);
            //            auto tree = AbstractSyntaxTree(str, InputType::kHaskell);
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            std::cout << tree.ExprToStringHaskell(tree.GetRoot()) << std::endl;
            assert(static_cast<int8_t>(term_size) == tree.CalculateTermSize(tree.GetRoot()));
            //            auto reductions = tree.NormalReduction(term_size * 50);
            //            if (!reductions.first) {
            //                no_normal_form_terms.push_back(reductions.second.front());
            //                continue;
            //            }
            //            cur_lengths.push_back(reductions.second.size() - 1);
        }
        double sum = 0;
        for (size_t idx = 0; idx < cur_lengths.size(); ++idx) {
            sum += cur_lengths[idx];
            //            std::cout<< cur_lengths[idx]<<" ";
        }
        //        std::cout<<"\n";
        average_lengths.push_back(sum / cur_lengths.size());
    }
    for (size_t idx = 0; idx < average_lengths.size(); ++idx) {
        std::cout << average_lengths[idx] << " ";
    }
    std::cout << "\n";
    for (auto &el : no_normal_form_terms) {
        std::cout << el << "\n";
    }
}

void FindMinBrokenTree() {
    int kek = 14;
    for (size_t term_size = 1; term_size <= kek; ++term_size) {
        std::vector<int64_t> cur_lengths;
        for (int64_t term_num = 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            std::cout << term_size << " " << 0 << " " << term_num << std::endl;
            //            auto str = terms_generator.GenerateTermStr(term_size, 0, term_num);
            //            auto tree = AbstractSyntaxTree(str, InputType::kHaskell);
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            std::cout << tree.ExprToStringHaskell(tree.GetRoot()) << std::endl;
//            auto reductions = tree.NormalReduction(5);
            //            if (!reductions.first) {
            //                no_normal_form_terms.push_back(reductions.second.front());
            //                continue;
            //            }
        }
    }
}

void DebugBuilding() {
    int n, m, k;
    std::cin >> n >> m >> k;
    auto str = terms_generator.GenerateTermStr(n, m, k);
    auto tree = AbstractSyntaxTree(str, InputType::kHaskell);
    auto tree1 = terms_generator.GenerateTerm(n, m, k);
    //    std::cout << tree1.ExprToStringHaskell(tree.GetRoot()) << std::endl;
//    auto reductions = tree1.NormalReduction(5);
}

void CalculateRatiosOfNormalForms() {
    size_t max_size_of_term = 8;
    std::vector<int64_t> no_normal_form_terms_count(max_size_of_term, 0);

    for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
        for (int64_t term_num = 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            auto reductions = tree.BetaReduction(StrategyType::kNormal, term_size * 20);
            if (!reductions.first) {
                no_normal_form_terms_count[term_size - 1]++;
                std::cout<<"Found one. " << term_size << " " << term_num <<std::endl;
            }
//            std::cout<<"Passed. " << term_size << " " << term_num <<std::endl;
        }
        for (auto &el : no_normal_form_terms_count) {
            std::cout << el << " ";
        }
        std::cout<<"\n";

        for (size_t idx = 0; idx < no_normal_form_terms_count.size(); ++idx) {
            std::cout<< static_cast<double>(no_normal_form_terms_count[idx])
                                 / terms_generator.GetCount(idx + 1, 0)<<" ";
        }
        std::cout<<std::endl;
    }
    for (auto &el : no_normal_form_terms_count) {
        std::cout << el << " ";
    }
    std::cout<<"\n";

    for (size_t idx = 0; idx < no_normal_form_terms_count.size(); ++idx) {
        std::cout<< static_cast<double>(no_normal_form_terms_count[idx])
                             / terms_generator.GetCount(idx + 1, 0)<<" ";
    }
    std::cout<<std::endl;
}

void TestFileOutput() {

}

int main() {
    CalculateRatiosOfNormalForms();

    TestFileOutput();

    //    AverageReductionChainLength();
    //        FindMinBrokenTree();
    //
    ////    DebugBuilding();
}
