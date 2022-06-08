#include "beta-reduction.h"
#include "include/TermGenerator.h"
#include <iostream>
#include <string>


TermGenerator terms_generator;

bool CreateTest(const std::string &test, const std::string &ans) {
    bool result;
    auto res = BetaReduction(test);
    if (*(--res.end()) == ans) {
        std::cout << "Correct! " << test << " reduced correctly to " << ans;
        result = true;
    } else {
        std::cout << " Oopsie. " << test << " reduced wrong to " << *(--res.end());
        result = false;
    }
    std::cout << "\n";

    return result;
}

bool RunTests() {
    bool is_passed = true;

    is_passed &= CreateTest("(\\x \\y x y y)(\\u u y x)", "\\y y y' x y");
    is_passed &= CreateTest("(\\x x x)(\\y \\z y z)", "\\z (\\z' z z')");
    is_passed &= CreateTest("\\x \\y (\\z (\\x z x) (\\y z y)) (x y)", "\\x (\\y (x y) (\\y' (x y) y'))");
    is_passed &= CreateTest("Omega a", "Term can't be reduced");
    is_passed &= CreateTest("(\\x x (x (y z)) x) (\\u u v)", "((y z) v) v (\\u u v)");
    is_passed &= CreateTest("First (Pair a b)", "a");
    is_passed &= CreateTest("(\\f \\x f (f x)) (\\x x)", "\\x x");
    is_passed &= CreateTest("(\\x \\y \\z x z (y z))((\\x \\y y x) u) ((\\x \\y y x) v) w", "w u (w v)");
    is_passed &= CreateTest("(\\x \\y \\z x z y)(\\x \\y x)", "\\y (\\z z)");
    is_passed &= CreateTest("(\\b \\x \\y b x y) False t e", "e");

    return is_passed;
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
                << "3. There are some checks for correct syntax, but they dont cover all possible mistake."
                   " Do your best to not make any mistakes"
                << "\n";
        std::cout << "4. The Interpreter supports 3 types of input: classic term, "
                     "term in de bruijn notation, haskell-style term"
                  <<"\n\t4.1 Classic term:"
                  << "\n\t\t4.1.1 Variables can only be named with single cursive letters"
                  << "\n\t\t4.1.2 Applications splits with spaces."
                  << "\n\t\t4.1.3 Abstractions looks like this: \\x z x y, where x is formal parameter "
                     "and 'z x y' is abstraction's body"
                  << "\n\t\t4.1.4 Example: (\\x x) (\\y y)"

                  <<"\n\t4.2 Term in de Bruijn notation:"
                  << "\n\t\t4.2.1 Variables can only be named with numbers"
                  << "\n\t\t4.2.2 Applications splits with spaces."
                  << "\n\t\t4.2.3 Abstractions looks like this: \\ 0 1 2, where '0 1 2' is abstraction's body;"
                     " 0 means bounded variable, 1 and 2 are some free vars"
                  << "\n\t\t4.2.4 Example: (\\ 0) (\\ 0)"

                  <<"\n\t4.3 Haskell-style term:"
                  << "\n\t\t4.3.1 Variables can only be named with numbers"
                  << "\n\t\t4.3.2 Applications splits with spaces."
                  << "\n\t\t4.3.3 Applications looks like this: App(term term)"
                  << "\n\t\t4.3.4 Abstractions looks like this: Abs(term)"
                  << "\n\t\t4.3.5 Example: App((Abs(0) (Abs(0))"<<"\n"
        ;

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
                std::cout << "\nOverflow with these parameters. Please choose smaller size of term and max count"
                          << "\n";
            }
        }
    }

    if (request == "gen") {
        size_t term_size, max_free_var_cnt;
        int64_t number_of_term;
        std::cout<<"Enter size of lambda-term and maximum count of free variables and number of term\n";
        std::cin >> term_size >> max_free_var_cnt>> number_of_term;
        std::cout << terms_generator.GenerateTermStr(term_size, max_free_var_cnt, number_of_term) << std::endl;
        return;
    }

    if (!IsSyntaxCorrect(request, InputType::kNormal)) {
        return;
    }

    //    auto output = BetaReduction(request);

    //    for (auto &element : output) {
    //        std::cout << " -> " << element << "\n";
    //    }
}


int main() {
    std::cout << "Created by @OganyanRV on 2022" << std::endl;
    std::cout << "Welcome to lambda-calculus interpreter. Choose the option:"
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

    while (true) {
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
