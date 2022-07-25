#include "include/TermGenerator.h"
#include "include/TermsLib.h"
#include <iomanip>
#include <iostream>
#include <string>


TermGenerator terms_generator;
TermsLib terms_lib;

std::vector<std::string> BetaReduction(std::string term, InputType input_type, StrategyType strategy_type) {
    if (input_type == InputType::kNormal) {
        term = terms_lib.ChangeLibFuncsToTerms(term);
    }

    AbstractSyntaxTree tree(term, input_type);

    if (strategy_type == StrategyType::kCallByValue) {
        auto tree1 = tree;
        std::cout << "Steps of beta-reduction in Call By Value strategy: " << std::endl;
        auto call_by_value_reduction = tree1.BetaReduction(StrategyType::kCallByValue);
        if (call_by_value_reduction.first != NormalFormType::kExisting) {
            call_by_value_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (call_by_value_reduction.first == NormalFormType::kCycling
                    ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                      : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                      " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
        }
        for (auto &step : call_by_value_reduction.second.first) {
            std::cout << step << std::endl;
        }
        return call_by_value_reduction.second.first;
    } else if (strategy_type == StrategyType::kCallByName) {
        auto tree2 = tree;
        std::cout << "Steps of beta-reduction in Call By Name strategy: " << std::endl;
        auto call_by_name_reduction = tree2.BetaReduction(StrategyType::kCallByName);
        if (call_by_name_reduction.first != NormalFormType::kExisting) {
            call_by_name_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (call_by_name_reduction.first == NormalFormType::kCycling
                                  ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                                  : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                                    " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
            return call_by_name_reduction.second.first;
        }
        for (auto &step : call_by_name_reduction.second.first) {
            std::cout << step << std::endl;
        }
        return call_by_name_reduction.second.first;
    } else if (strategy_type == StrategyType::kNormal) {
        auto tree3 = tree;
        std::cout << "Steps of beta-reduction in Normal strategy: " << std::endl;
        auto normal_reduction = tree3.BetaReduction(StrategyType::kNormal);
        if (normal_reduction.first != NormalFormType::kExisting) {
            normal_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (normal_reduction.first == NormalFormType::kCycling
                                  ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                                  : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                                    " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
            return normal_reduction.second.first;
        }
        for (auto &step : normal_reduction.second.first) {
            std::cout << step << std::endl;
        }
        return normal_reduction.second.first;
    } else if (strategy_type == StrategyType::kAll) {
        auto tree1 = tree;
        std::cout << "Steps of beta-reduction in Call By Value strategy: " << std::endl;
        auto call_by_value_reduction = tree1.BetaReduction(StrategyType::kCallByValue);
        if (call_by_value_reduction.first != NormalFormType::kExisting) {
            call_by_value_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (call_by_value_reduction.first == NormalFormType::kCycling
                                  ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                                  : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                                    " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
        } else {
            for (auto &step : call_by_value_reduction.second.first) {
                std::cout << step << std::endl;
            }
        }
        auto tree2 = tree;
        std::cout << "Steps of beta-reduction in Call By Name strategy: " << std::endl;
        auto call_by_name_reduction = tree2.BetaReduction(StrategyType::kCallByName);
        if (call_by_name_reduction.first != NormalFormType::kExisting) {
            call_by_name_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (call_by_name_reduction.first == NormalFormType::kCycling
                                  ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                                  : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                                    " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
        } else {
            for (auto &step : call_by_name_reduction.second.first) {
                std::cout << step << std::endl;
            }
        }
        auto tree3 = tree;
        std::cout << "Steps of beta-reduction in Normal strategy: " << std::endl;
        auto normal_reduction = tree3.BetaReduction(StrategyType::kNormal);
        if (normal_reduction.first != NormalFormType::kExisting) {
            normal_reduction.second.first.push_back("Term has no normal form");
            std::cout << "Term has no normal form" << std::endl;
            std::cout << (normal_reduction.first == NormalFormType::kCycling
                                  ? "Cycling type of non-existence of normal form. Term for sure does not have normal form"
                                  : "Increasing type of non-existence of normal form. Existence of normal form depends on limits such as"
                                    " steps_count_limit and max_size_of_reduced_term_limit")<<std::endl;
        } else {
            for (auto &step : normal_reduction.second.first) {
                std::cout << step << std::endl;
            }
        }
        return normal_reduction.second.first;
    }
    return {};
}

bool CreateTest(const std::string &test, const std::string &ans) {
    bool result;
    auto res = BetaReduction(test, InputType::kNormal, StrategyType::kNormal);
    if (*(--res.end()) == ans) {
        std::cout << "Correct! " << test << " reduced correctly to " << ans;
        result = true;
    } else {
        std::cout << "Wrong! " << test << " reduced wrong to " << *(--res.end());
        result = false;
    }
    std::cout << "\n";

    return result;
}

bool RunTests() {
    bool is_passed = true;

    is_passed &= CreateTest("(\\x \\y x y y)(\\u u y x)", "(Abs (App (App (App 0 1) 2) 0))");
    is_passed &= CreateTest("(\\x x x)(\\y \\z y z)", "(Abs (Abs (App 1 0)))");
    is_passed &= CreateTest("\\x \\y (\\z (\\x z x) (\\y z y)) (x y)", "(Abs (Abs (App (App 1 0) (Abs (App (App 2 1) 0)))))");
    is_passed &= CreateTest("Omega a", "Term has no normal form");
    is_passed &= CreateTest("(\\x x (x (y z)) x) (\\u u v)", "(App (App (App (App 0 1) 2) 2) (Abs (App 0 3)))");
    is_passed &= CreateTest("First (Pair a b)", "0");
    is_passed &= CreateTest("(\\f \\x f (f x)) (\\x x)", "(Abs 0)");
    is_passed &= CreateTest("(\\x \\y \\z x z (y z))((\\x \\y y x) u) ((\\x \\y y x) v) w", "(App (App 2 0) (App 2 1))");
    is_passed &= CreateTest("(\\x \\y \\z x z y)(\\x \\y x)", "(Abs (Abs 0))");
    is_passed &= CreateTest("(\\b \\x \\y b x y) False t e", "1");

    return is_passed;
}

void StartBetaReductionMenu() {
    while (true) {
        std::cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
        std::cout << "Beta-reduction's interpreter. Options:\n"
                  << "1. There are 2 possible types of input:\n"
                  << "\t1.1 Type '1' for classic term style. Example: (\\x x) (\\y y)\n"
                  << "\t1.2 Type '2' for Haskell-style term. Example: (App((Abs(0) (Abs(0)))\n"
                  << "2. Type 'exit' to exit beta-reduction's interpreter\n";
        std::string choice;
        std::cin >> choice;
        if (choice == "exit") {
            return;
        }

        InputType input_type;
        if (choice == "1") {
            input_type = InputType::kNormal;
        } else if (choice == "2") {
            input_type = InputType::kHaskell;
        } else {
            continue;
        }

        std::cout << "Enter term:\n";
        std::string term;
        while (term.empty()) {
            getline(std::cin, term);
        }

        std::cout << "1. Type '1' for normal strategy reduction\n"
                  << "2. Type '2' for call by name strategy reduction\n"
                  << "3. Type '3' for call by value reduction\n"
                  << "4. Type '4' for all strategies reductions\n";

        StrategyType strategy_type;
        while (true) {
            std::cin >> choice;
            if (choice == "1") {
                strategy_type = StrategyType::kNormal;
            } else if (choice == "2") {
                strategy_type = StrategyType::kCallByName;
            } else if (choice == "3") {
                strategy_type = StrategyType::kCallByValue;
            } else if (choice == "4") {
                strategy_type = StrategyType::kAll;
            } else {
                continue;
            }
            break;
        }

        BetaReduction(term, input_type, strategy_type);
    }
}

void ProcessRequest(const std::string &request) {
    if (request == "rules") {
        std::cout << "-+-+-+-+-+-+-+-+-+-+  Rules  -+-+-+-+-+-+-+-+-+-+-+-+-+"
                  << "\n";
        std::cout << "1. Capital letters are only used for Library Functions such as True, False, Not ..."
                  << "\n";
        std::cout << "2. Be accurate with brackets. Do not mess up with brackets"
                  << "\n";
        std::cout
                << "3. There are some checks for correct syntax, but they dont cover all possible mistakes."
                   " Do your best to not make any mistakes"
                << "\n";
        std::cout << "4. The Interpreter supports 2 types of input: classic term "
                     "and haskell-style term"
                  << "\n\t4.1 Classic term:"
                  << "\n\t\t4.1.1 Variables can only be named with single cursive letters"
                  << "\n\t\t4.1.2 Applications split with spaces."
                  << "\n\t\t4.1.3 Abstractions look like this: \\x z x y, where x is formal parameter "
                     "and 'z x y' is abstraction's body"
                  << "\n\t\t4.1.4 Example: (\\x x) (\\y y)"

                  << "\n\t4.2 Haskell-style term:"
                  << "\n\t\t4.2.1 Variables can only be named with numbers"
                  << "\n\t\t4.2.2 Applications split with spaces."
                  << "\n\t\t4.2.3 Applications look like this: App(term term)"
                  << "\n\t\t4.2.4 Abstractions look like this: Abs(term)"
                  << "\n\t\t4.2.5 Example: (App((Abs(0) (Abs(0)))"
                  << "\n";

        return;
    }

    if (request == "test") {
        std::cout << "-+-+-+-+-+-+-+-+-+-+  Running tests  -+-+-+-+-+-+-+-+-+-+-+-+-+-+"
                  << "\n";
        if (RunTests()) {
            std::cout << "Tests passed successfully!"
                      << "\n";
        } else {
            std::cout << "Some of tests did not pass"
                      << "\n";
        }
        return;
    }

    if (request == "count") {
        while (true) {
            std::cout << "Options: \n"
                      << "Type '1' to print table of terms counts \n"
                      << "Type '2' to print count of possible terms with exact parameters\n";
            int choice = 0;
            while (choice != 1 && choice != 2) {
                std::cin >> choice;
            }
            if (choice == 2) {
                while (true) {
                    size_t term_size, max_free_vars_count;
                    std::cout << "Enter size of lambda-term and maximum count of free variables"
                              << "\n";
                    std::cout << "Enter size of lambda-term: ";
                    std::cin >> term_size;
                    std::cout << "Enter maximum count of free variables: ";
                    std::cin >> max_free_vars_count;

                    if (TermGenerator::CheckIfAllowed(term_size, max_free_vars_count)) {
                        int64_t cnt = terms_generator.GetCount(term_size, max_free_vars_count);
                        std::cout << "\nCount of lambda-terms with length " << term_size << " and max count of free variables "
                                  << max_free_vars_count << " is: " << cnt << std::endl;
                        return;
                    } else {
                        std::cout << "Overflow with these parameters. Please choose smaller size of term and max count"
                                  << "\n";
                    }
                }
            } else {
                std::cout << "n\\m \t";
                for (size_t max_free_vars_cnt = 0; max_free_vars_cnt <= TermGenerator::kMaxFreeVarsCount; ++max_free_vars_cnt) {
                    std::cout << std::setw(20) << max_free_vars_cnt;
                }
                std::cout << "\n";

                for (size_t size_of_term = 0; size_of_term <= TermGenerator::kMaxTermSize; ++size_of_term) {
                    std::cout << std::setw(2) << size_of_term << "  \t";
                    for (size_t max_free_vars_cnt = 0; max_free_vars_cnt <= TermGenerator::kMaxFreeVarsCount; ++max_free_vars_cnt) {
                        if (max_free_vars_cnt + size_of_term <= 18) {
                            std::cout << std::setw(20)
                                      << terms_generator.GetCount(size_of_term, max_free_vars_cnt);
                        }
                    }
                    std::cout << "\n";
                }
                std::cout << "\n";
                return;
            }
        }
    }

    if (request == "gen") {
        while (true) {
            size_t term_size, max_free_vars_count;
            int64_t number_of_term = 0;
            std::cout << "Enter size of lambda-term, maximum count of free variables and number of term\n";
            std::cout << "Enter size of lambda-term: ";
            std::cin >> term_size;
            std::cout << "Enter maximum count of free variables: ";
            std::cin >> max_free_vars_count;
            if (TermGenerator::CheckIfAllowed(term_size, max_free_vars_count)) {
                std::cout << "Count of all possible terms with those parameters: "
                          << terms_generator.GetCount(term_size, max_free_vars_count) << "\n";
                std::cout << "Enter number of term\n";
                while (number_of_term <= 0 || number_of_term > terms_generator.GetCount(term_size, max_free_vars_count)) {
                    std::cin >> number_of_term;
                }
                std::cout << terms_generator.GenerateTermStr(term_size, max_free_vars_count, number_of_term) << std::endl;
                return;
            } else {
                std::cout << "Overflow with these parameters. Please choose smaller size of term and max count"
                          << "\n";
            }
        }
    } else if (request == "start") {
        StartBetaReductionMenu();
    }
}

int main() {
    std::cout << "Created by @OganyanRV in 2022" << std::endl;
    while (true) {
        std::cout << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
        std::cout << "Choose the option of lambda-calculus interpreter:"
                  << "\n"
                  << "1. Type 'rules' to read rules of input"
                  << "\n"
                  << "2. Type 'test' to run implemented tests"
                  << "\n"
                  << "3. Type 'count' to calculate count of possible terms"
                  << "\n"
                  << "4. Type 'gen' to generate term"
                  << "\n"
                  << "5. Type 'start' to enter beta-reduction's menu"
                  << "\n"
                  << "6. Type 'exit' to quit the program"
                  << "\n";


        std::cout << "> ";
        std::string input;
        while (input.empty()) {
            getline(std::cin, input);
        }
        if (input == "exit") {
            break;
        }
        ProcessRequest(input);
    }
    return 0;
}
