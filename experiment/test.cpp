#include "../include/TermGenerator.h"
#include <cassert>
#include <iostream>
#include <string>

#include <filesystem>
#include <fstream>

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


void CalculateRatiosOfNormalForms(const std::filesystem::path &path) {
    //    bool is_new_file = !std::filesystem::exists(path);
    //std::ios::in | std::iso::app
    std::ofstream file(path, std::ios::trunc);
    file << "size_of_term,number_of_term,"
            "steps_count,size_biggest_term_in_steps,size_reduced,"
            "cycled_reduction,increasing_reduction,has_normal_form\n";


    size_t max_size_of_term = 7;

    for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
        for (int64_t term_num = 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            auto reductions = tree.BetaReduction(StrategyType::kNormal, term_size * 15, 65536);
            file << term_size << "," << term_num << ",";
            if (!reductions.first) {
                if (reductions.second.second.size() >= term_size * 15 - 1) {
                    file << ",,,"
                         << "False,True,False";
                } else {
                    file << ",,,"
                         << "True,False,False";
                }

            } else {
                int64_t biggest_term_in_step = 0;
                auto &steps_size = reductions.second.second;
                for (size_t idx = 0; idx < steps_size.size(); ++idx) {
                    biggest_term_in_step = std::max(biggest_term_in_step, steps_size[idx]);
                }
                file << steps_size.size() - 1 << "," << biggest_term_in_step << "," << *(--steps_size.end()) << ",False,False,True";
            }
            file << "\n";
        }
    }
}

void TestFileOutput() {
    /*
 * size_t max_size_of_term = 7;
std::vector<int64_t> no_normal_form_terms_count(max_size_of_term, 0);

for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
    for (int64_t term_num = 1;
         term_num <= terms_generator.GetCount(term_size, 0);
         ++term_num) {
        auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
        auto reductions = tree.BetaReduction(StrategyType::kNormal, term_size * 15, 65536);
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
 */
}

int main() {
    std::filesystem::path base_path{"/home/ogrob/diplom/Diploma/experiment"};

    CalculateRatiosOfNormalForms(base_path / "normal_form_ratios.csv");
    //    AverageReductionChainLength(base_path / "normal_form_ratios.csv");

    //    TestFileOutput();

    //    AverageReductionChainLength();
    //        FindMinBrokenTree();
}
