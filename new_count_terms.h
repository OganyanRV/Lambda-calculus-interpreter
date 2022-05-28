#include <vector>

#ifndef OGANYAN_LAMBDA_CALC_NEW_COUNT_TERMS_H
#define OGANYAN_LAMBDA_CALC_NEW_COUNT_TERMS_H

class CountOfTerms {
private:
    std::vector<std::vector<int64_t>> table_;
    static const int N = 18;
    static const int M = 18;

    int64_t CalculateTermsCount(size_t term_size, size_t free_variables_size) {
        int64_t count_of_terms = table_[term_size - 1][free_variables_size + 1];
        int64_t sum = 0;
        for (size_t idx = 0; idx < term_size; ++idx) {
            sum += table_[idx][free_variables_size] * table_[term_size - 1 - idx][free_variables_size];
        }
        count_of_terms += sum;

        return count_of_terms;
    }

    void FillTable() {

        for (size_t term_size_idx = 1; term_size_idx <= N; ++term_size_idx) {
            for (size_t free_var_idx = 0; free_var_idx <M; ++free_var_idx) {
                if (term_size_idx + free_var_idx <= N) {
                    auto kek = CalculateTermsCount(term_size_idx, free_var_idx);
                    //                std::cout<<kek<<" ";
                    table_[term_size_idx][free_var_idx] = kek;
                }
            }
//            std::cout<<"\n";
        }
    }

public:
    CountOfTerms() : table_(std::vector<std::vector<int64_t>>(N + 1, std::vector<int64_t>(M + 1, 0))) {
        for (size_t idx = 0; idx <= M; ++idx) {
            table_[0][idx] = static_cast<int64_t>(idx);
        }

        FillTable();

    };

    int64_t GetCount(size_t term_size, size_t free_variables_size) {
        return table_[term_size][free_variables_size];
    }

    static bool CheckIfAllowed(size_t term_size, size_t free_variables_size) {
        if (term_size + free_variables_size > N) {
            return false;
        }
        return true;
    }

};

#endif //OGANYAN_LAMBDA_CALC_NEW_COUNT_TERMS_H
