#ifndef OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
#define OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
#include <vector>
#include <cstdint>
#include <string>
#include "AbstractSyntaxTree.h"

class TermGenerator {
public:
    static const int kMaxTermSize = 18;
    static const int kMaxFreeVarsCount = 18;
private:
    std::vector<std::vector<int64_t>> terms_count_table_;

    int64_t CalculateTermsCount(size_t term_size, size_t free_variables_size);
    void FillTable();

    std::string AppTermStr(size_t max_free_var_cnt, size_t term_size, size_t j, int64_t h);
    std::string UnRankTStr(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term);

    std::shared_ptr<TermNode> AppTerm(size_t max_free_var_cnt, size_t term_size, size_t j, int64_t h, std::shared_ptr<TermNode> from);
    std::shared_ptr<TermNode> UnRankT(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term, std::shared_ptr<TermNode> from);

public:
    TermGenerator();
    int64_t GetCount(size_t term_size, size_t free_variables_size);
    static bool CheckIfAllowed(size_t term_size, size_t free_variables_size);
    std::string GenerateTermStr(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term);
    AbstractSyntaxTree GenerateTerm(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term);
};

#endif//OGANYAN_LAMBDA_CALC_TERMGENERATOR_H
