// Copyright 2022 Oganyan Robert

#include "include/TermGenerator.h"
#include "beta-reduction.h"
#include <iostream>
#include <string>

//#define Show_Steps_At_Tests
#define Show_Steps_At_Interpreter

TermGenerator cnt_of_terms;

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

#ifdef Show_Steps_At_Tests
    cout << "Showing steps: "
         << "\n";
    for (auto &element : res) {
        cout << " -> " << element << "\n";
    }
#endif

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
        std::cout << "2. Variables can be named only with cursive letters or words fully consisting of cursive letters"
                  << "\n";
        std::cout << "3. Do not mess up with brackets"
                  << "\n";
        std::cout << "4. Lambda is '\\x'; Expression splits with spaces, for example '(\\x x y) a -> a y' "
                  << "\n";
        std::cout
                << "5. There are some checks for correct syntax, but not for every possible mistake. Do your best to not make any mistakes"
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
            size_t n, m;
            std::cout << "Enter length of lambda-term and maximum count of free variables"
                      << "\n";
            std::cout << "Length of lambda-term: ";
            std::cin >> n;
            std::cout << "Maximum count of free variables: ";
            std::cin >> m;

            if (TermGenerator::CheckIfAllowed(n, m)) {
                int64_t cnt = cnt_of_terms.GetCount(n, m);
                std::cout << "\nCount of lambda-terms with length " << n << " and max count of free variables "
                          << m << " is: " << cnt << std::endl;
                return;
            } else {
                std::cout << "\nOverflow with these parameters. Please choose smaller length and max count"
                          << "\n";
            }
        }
    }

    if (request == "gen") {
        size_t n, m, k;
        std::cin >> n >> m >> k;
        std::cout << cnt_of_terms.UnRankT(n, m, k) << std::endl;
        return;
    }

    if (!IsSyntaxCorrect(request)) {
        return;
    }

    auto output = BetaReduction(request);

#ifdef Show_Steps_At_Interpreter
    for (auto &element : output) {
        std::cout << " -> " << element << "\n";
    }

#endif

#ifndef Show_Steps_At_Interpreter
    std::cout << " -> " << *(--output.end()) << "\n";
#endif
}

void FastOutput() {
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::ios::sync_with_stdio(false);
}

int main() {
    std::cout << "Welcome to beta-reduction calculator. Choose the option:"
              << "\n"
              << "1. Type 'rules' to read rules for syntax "
              << "\n"
              << "2. Type 'test' to run implemented tests"
              << "\n"
              << "3. Type the lambda term to reduce it"
              << "\n"
              << "4. Type 'count' to calculate count of terms"
              << "\n"
              << "5. Type 'exit' to quit the program"
              << "\n"
              << "6. Type 'gen' to generate term"
              << "\n";

    while (true) {
        //        FastOutput();
        std::cout << ">"
                  << " ";
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
