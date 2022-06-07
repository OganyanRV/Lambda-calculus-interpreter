#ifndef OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
#define OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
#include <vector>
#include <cstdint>
#include <string>

class TermGenerator {
private:
    std::vector<std::vector<int64_t>> terms_count_table_;
    static const int kMaxTermSize = 18;
    static const int kMaxFreeVarsCount = 18;

    int64_t CalculateTermsCount(size_t term_size, size_t free_variables_size);
    void FillTable();

public:
    TermGenerator();
    int64_t GetCount(size_t term_size, size_t free_variables_size);
    static bool CheckIfAllowed(size_t term_size, size_t free_variables_size);
    std::string AppTerm(size_t m, size_t n, size_t j, int64_t h);
    std::string UnRankT(size_t n, size_t m, int64_t k);
};

#endif//OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
