#include "../beta-reduction.h"
#include "../include/TermGenerator.h"
#include <iostream>
#include <string>

TermGenerator terms_generator;

int main() {
    while(true) {
        size_t term_size, max_free_var_cnt;
        int64_t number_of_term;
        std::cin >> term_size >> max_free_var_cnt >> number_of_term;
        std::cout << terms_generator.GenerateTermStr(term_size, max_free_var_cnt, number_of_term) << std::endl;

        auto tree = terms_generator.GenerateTerm(term_size, max_free_var_cnt, number_of_term);
        std::cout<<tree.ExprToStringHaskell(tree.GetRoot()) <<std::endl;
    }
}
