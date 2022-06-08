#include "../beta-reduction.h"
#include "../include/TermGenerator.h"
#include <iostream>
#include <string>

TermGenerator terms_generator;

void AverageReductionChainLength() {
    std::vector<double> average_lengths;
    int kek = 7;
    for (size_t term_size = 1; term_size <= kek; ++term_size) {
        std::vector<int64_t> cur_lengths;
        for (int64_t term_num = terms_generator.GetCount(term_size - 1, 1);
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            std::cout << tree.ExprToStringHaskell(tree.GetRoot()) << std::endl;
            auto reductions = tree.NormalReduction();
            for (auto &step : reductions) {
                std::cout << step << "\n";
            }
            cur_lengths.push_back(reductions.size() - 1);
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
        std::cout<< average_lengths[idx]<<" ";
    }
}
int main() {
    AverageReductionChainLength();
//    while (true) {
//        size_t term_size, max_free_var_cnt;
//        int64_t number_of_term;
//        std::cin >> term_size >> max_free_var_cnt >> number_of_term;
//        std::cout << terms_generator.GenerateTermStr(term_size, max_free_var_cnt, number_of_term) << std::endl;
//
//        auto tree = terms_generator.GenerateTerm(term_size, max_free_var_cnt, number_of_term);
//        std::cout << tree.ExprToStringHaskell(tree.GetRoot()) << std::endl;
//        auto reductions = tree.NormalReduction();
//        for (auto &step : reductions) {
//            std::cout << step << "\n";
//        }
//        //        std::cout<<tree.ExprToStringHaskell(tree.GetRoot()) <<std::endl;
//        //        return 0;
//    }
}
