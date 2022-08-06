#include "../include/TermGenerator.h"
#include <iostream>
#include <string>

#include <filesystem>
#include <fstream>
#include <random>
#include <set>

TermGenerator terms_generator;

void Reduction(std::ofstream &file_norm, std::ofstream &file_val, std::ofstream &file_name,
               size_t term_size, int64_t term_num, int64_t limit) {
    auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);

    auto tree_norm = tree;
    auto tree_val = tree;
    auto tree_name = tree;

    file_norm << term_size << "," << term_num << ",";
    file_val << term_size << "," << term_num << ",";
    file_name << term_size << "," << term_num << ",";


    auto reductions_norm = tree.BetaReduction(
            StrategyType::kNormal, term_size * 15, limit);

    if (reductions_norm.first != NormalFormType::kExisting) {
        if (reductions_norm.first == NormalFormType::kIncreasing) {
            file_norm << ",,,"
                      << "False,True,False";
        } else {
            file_norm << ",,,"
                      << "True,False,False";
        }
    } else {
        int64_t biggest_term_in_step = 0;
        auto &steps_size = reductions_norm.second.second;
        for (size_t idx = 0; idx < steps_size.size(); ++idx) {
            biggest_term_in_step = std::max(biggest_term_in_step, steps_size[idx]);
        }
        file_norm << steps_size.size() - 1 << "," << biggest_term_in_step << "," << *(--steps_size.end()) << ",False,False,True";
    }


    auto reductions_val = tree_val.BetaReduction(
            StrategyType::kCallByValue, term_size * 15, limit);

    if (reductions_val.first != NormalFormType::kExisting) {
        if (reductions_val.first == NormalFormType::kIncreasing) {
            file_val << ",,,"
                     << "False,True,False";
        } else {
            file_val << ",,,"
                     << "True,False,False";
        }

    } else {
        int64_t biggest_term_in_step = 0;
        auto &steps_size = reductions_val.second.second;
        for (size_t idx = 0; idx < steps_size.size(); ++idx) {
            biggest_term_in_step = std::max(biggest_term_in_step, steps_size[idx]);
        }
        file_val << steps_size.size() - 1 << "," << biggest_term_in_step << "," << *(--steps_size.end()) << ",False,False,True";
    }

    auto reductions_name = tree_name.BetaReduction(
            StrategyType::kCallByName, term_size * 15, limit);

    if (reductions_name.first != NormalFormType::kExisting) {
        if (reductions_name.first == NormalFormType::kIncreasing) {
            file_name << ",,,"
                      << "False,True,False";
        } else {
            file_name << ",,,"
                      << "True,False,False";
        }

    } else {
        int64_t biggest_term_in_step = 0;
        auto &steps_size = reductions_name.second.second;
        for (size_t idx = 0; idx < steps_size.size(); ++idx) {
            biggest_term_in_step = std::max(biggest_term_in_step, steps_size[idx]);
        }
        file_name << steps_size.size() - 1 << "," << biggest_term_in_step << "," << *(--steps_size.end()) << ",False,False,True";
    }

    file_norm << ",True";
    if (*(--reductions_val.second.first.end()) == *(--reductions_norm.second.first.end())) {
        file_norm << ",True";
        file_val << ",True,True";
    } else {
        file_norm << ",False";
        file_val << ",False,True";
    }

    if (*(--reductions_name.second.first.end()) == *(--reductions_norm.second.first.end())) {
        file_norm << ",True";
        file_name << ",True";
    } else {
        file_norm << ",False";
        file_name << ",False";
    }

    if (*(--reductions_name.second.first.end()) == *(--reductions_val.second.first.end())) {
        file_val << ",True";
        file_name << ",True,True";
    } else {
        file_val << ",False";
        file_name << ",False,True";
    }

    file_norm << "\n";
    file_val << "\n";
    file_name << "\n";
}

void AverageReductionChainLength(const std::filesystem::path &path_norm,
                                 const std::filesystem::path &path_val,
                                 const std::filesystem::path &path_name) {
    std::ofstream file_norm(path_norm, std::ios::trunc);
    file_norm << "size_of_term,number_of_term,"
                 "steps_count,size_biggest_term_in_steps,size_reduced,"
                 "cycled_reduction,increasing_reduction,has_normal_form,"
              << "is_same_norm,is_same_val,is_same_name\n";

    std::ofstream file_val(path_val, std::ios::trunc);
    file_val << "size_of_term,number_of_term,"
                "steps_count,size_biggest_term_in_steps,size_reduced,"
                "cycled_reduction,increasing_reduction,has_normal_form,"
             << "is_same_norm,is_same_val,is_same_name\n";

    std::ofstream file_name(path_name, std::ios::trunc);
    file_name << "size_of_term,number_of_term,"
                 "steps_count,size_biggest_term_in_steps,size_reduced,"
                 "cycled_reduction,increasing_reduction,has_normal_form,"
              << "is_same_norm,is_same_val,is_same_name\n";

    for (size_t term_size = 1; term_size <= 9; ++term_size) {
        std::cout << "started " << term_size << std::endl;

        for (int64_t term_num = terms_generator.GetCount(term_size - 1, 1);
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {

            Reduction(file_norm, file_val, file_name, term_size, term_num, 2048);
        }
    }

    for (size_t term_size = 10; term_size <= 12; ++term_size) {
        std::cout << "started " << term_size << std::endl;
        std::mt19937 gen(154461);
        std::uniform_int_distribution<size_t> dist(terms_generator.GetCount(term_size - 1, 1),
                                                   terms_generator.GetCount(term_size, 0));
        std::set<size_t> random_terms_indexes;
        while (random_terms_indexes.size() != 500000) {
            random_terms_indexes.insert(dist(gen));
        }

        for (auto &term_num : random_terms_indexes) {
            Reduction(file_norm, file_val, file_name, term_size, term_num, 4096);
        }
    }

    for (size_t term_size = 13; term_size <= 15; ++term_size) {
        std::cout << "started " << term_size << std::endl;
        std::mt19937 gen(154461);
        std::uniform_int_distribution<size_t> dist(terms_generator.GetCount(term_size - 1, 1),
                                                   terms_generator.GetCount(term_size, 0));
        std::set<size_t> random_terms_indexes;
        while (random_terms_indexes.size() != 250000) {
            random_terms_indexes.insert(dist(gen));
        }

        for (auto &term_num : random_terms_indexes) {
            Reduction(file_norm, file_val, file_name, term_size, term_num, 8192);
        }
    }

    for (size_t term_size = 16; term_size <= 18; ++term_size) {
        std::cout << "started " << term_size << std::endl;
        std::mt19937 gen(154461);
        std::uniform_int_distribution<size_t> dist(terms_generator.GetCount(term_size - 1, 1),
                                                   terms_generator.GetCount(term_size, 0));
        std::set<size_t> random_terms_indexes;
        while (random_terms_indexes.size() != 100000) {
            random_terms_indexes.insert(dist(gen));
        }

        for (auto &term_num : random_terms_indexes) {
            Reduction(file_norm, file_val, file_name, term_size, term_num, 12000);
        }
    }
}


void CalculateRatiosOfNormalForms(const std::filesystem::path &path, size_t max_size_of_term = 10) {
    //std::ios::in | std::iso::app
    std::ofstream file(path, std::ios::trunc);
    file << "size_of_term,number_of_term,"
            "steps_count,size_biggest_term_in_steps,size_reduced,"
            "cycled_reduction,increasing_reduction,has_normal_form\n";

    size_t term_size_limit = 2048;

    for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
        std::cout << "started " << term_size << std::endl;
        if (term_size == 10) {
            term_size_limit = 4096;
        }
        for (int64_t term_num = 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            auto reductions = tree.BetaReduction(StrategyType::kNormal, term_size * 15, term_size_limit);
            file << term_size << "," << term_num << ",";
            if (reductions.first != NormalFormType::kExisting) {
                if (reductions.first == NormalFormType::kIncreasing) {
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

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void ArticleAverageReductionChainLength(const std::filesystem::path &path_norm,
                                        const std::filesystem::path &path_val,
                                        const std::filesystem::path &path_name) {
    std::ofstream file_norm(path_norm, std::ios::trunc);
    file_norm << "size_of_term,number_of_term,"
                 "steps_count,size_biggest_term_in_steps,size_reduced,"
                 "cycled_reduction,increasing_reduction,has_normal_form,"
              << "is_same_norm,is_same_val,is_same_name\n";

    std::ofstream file_val(path_val, std::ios::trunc);
    file_val << "size_of_term,number_of_term,"
                "steps_count,size_biggest_term_in_steps,size_reduced,"
                "cycled_reduction,increasing_reduction,has_normal_form,"
             << "is_same_norm,is_same_val,is_same_name\n";

    std::ofstream file_name(path_name, std::ios::trunc);
    file_name << "size_of_term,number_of_term,"
                 "steps_count,size_biggest_term_in_steps,size_reduced,"
                 "cycled_reduction,increasing_reduction,has_normal_form,"
              << "is_same_norm,is_same_val,is_same_name\n";

    std::vector<size_t> limits(11);
    for (size_t idx = 0; idx < 11; ++idx) {
        limits[idx] = 120 * (idx + 1);
    }
    for (size_t term_size = 1; term_size <= 11; ++term_size) {
        std::cout << "started " << term_size << std::endl;

        for (int64_t term_num = terms_generator.GetCount(term_size - 1, 1) + 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            if (term_num == 578	) {
                std::cout<<"KEK"<<std::endl;
                int kok = 0;
            }
            Reduction(file_norm, file_val, file_name, term_size, term_num, limits[term_size - 1]);
        }
    }
}

void ArticleCalculateRatiosOfNormalForms(const std::filesystem::path &path, size_t max_size_of_term = 11) {
    //std::ios::in | std::iso::app
    std::ofstream file(path, std::ios::trunc);
    file << "size_of_term,number_of_term,"
            "steps_count,size_biggest_term_in_steps,size_reduced,"
            "cycled_reduction,increasing_reduction,has_normal_form\n";

    std::vector<size_t> limits(max_size_of_term);
    for (size_t idx = 0; idx < max_size_of_term; ++idx) {
        limits[idx] = 120 * (idx + 1);
    }

    for (size_t term_size = 1; term_size <= max_size_of_term; ++term_size) {
        std::cout << "started " << term_size << std::endl;

        for (int64_t term_num = 1;
             term_num <= terms_generator.GetCount(term_size, 0);
             ++term_num) {
            if (term_num == 578	) {
                std::cout<<"KEK"<<std::endl;
                int kok = 0;
            }
            auto tree = terms_generator.GenerateTerm(term_size, 0, term_num);
            auto reductions = tree.BetaReduction(StrategyType::kNormal, term_size * 15, limits[term_size - 1]);
            file << term_size << "," << term_num << ",";
            if (reductions.first != NormalFormType::kExisting) {
                if (reductions.first == NormalFormType::kIncreasing) {
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


#define Article_exp
int main() {
    // Diploma experiments
#ifndef Article_exp
    std::filesystem::path base_path{"/home/ogrob/oganyan_diploma/Diploma/experiment"};

    AverageReductionChainLength(base_path / "normal_strat.csv",
                                base_path / "call_by_value.csv",
                                base_path / "call_by_name.csv");

    CalculateRatiosOfNormalForms(base_path / "normal_form_ratios.csv");
#endif

    // Article experiments
#ifdef Article_exp

    std::filesystem::path base_path{"/home/ogrob/oganyan_diploma/Diploma/experiment/article"};

    ArticleCalculateRatiosOfNormalForms(base_path / "normal_form_ratios.csv", 5);
    std::cout<<"Exp 1 done"<<std::endl;

    ArticleAverageReductionChainLength(base_path / "normal_strat.csv",
                                       base_path / "call_by_value.csv",
                                       base_path / "call_by_name.csv");
    std::cout<<"Exp 2 done"<<std::endl;

#endif
}
