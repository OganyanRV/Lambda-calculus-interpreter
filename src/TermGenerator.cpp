#include "../include/TermGenerator.h"


int64_t TermGenerator::CalculateTermsCount(size_t term_size, size_t free_variables_size) {
    int64_t count_of_terms = terms_count_table_[term_size - 1][free_variables_size + 1];
    int64_t sum = 0;
    for (size_t idx = 0; idx < term_size; ++idx) {
        sum += terms_count_table_[idx][free_variables_size] * terms_count_table_[term_size - 1 - idx][free_variables_size];
    }
    count_of_terms += sum;

    return count_of_terms;
}

void TermGenerator::FillTable() {
    for (size_t term_size_idx = 1; term_size_idx <= kMaxTermSize; ++term_size_idx) {
        for (size_t free_var_idx = 0; free_var_idx < kMaxFreeVarsCount; ++free_var_idx) {
            if (term_size_idx + free_var_idx <= kMaxTermSize) {
                terms_count_table_[term_size_idx][free_var_idx] = CalculateTermsCount(term_size_idx, free_var_idx);
            }
        }
    }
}

TermGenerator::TermGenerator() : terms_count_table_(std::vector<std::vector<int64_t>>(
                                         kMaxTermSize + 1,
                                         std::vector<int64_t>(kMaxFreeVarsCount + 1, 0))) {
    for (size_t idx = 0; idx <= kMaxFreeVarsCount; ++idx) {
        terms_count_table_[0][idx] = static_cast<int64_t>(idx);
    }

    FillTable();
}

int64_t TermGenerator::GetCount(size_t term_size, size_t free_variables_size) {
    return terms_count_table_[term_size][free_variables_size];
}

bool TermGenerator::CheckIfAllowed(size_t term_size, size_t free_variables_size) {
    if (term_size + free_variables_size > kMaxTermSize) {
        return false;
    }
    return true;
}

std::string TermGenerator::AppTerm(size_t m, size_t n, size_t j, int64_t h) {
    int64_t tnjm = GetCount(n - j, m);
    int64_t tjmtnjm = GetCount(j, m) * tnjm;

    if (h <= tjmtnjm) {
        int64_t dv = (h - 1) / tnjm;
        int64_t md = (h - 1) % tnjm;
        std::string left_term = UnRankT(j, m, dv + 1);
        std::string right_term = UnRankT(n - j, m, md + 1);
        return std::string("(App ") + left_term + " " + right_term + ") ";
    } else {
        return AppTerm(m, n, j + 1, h - tjmtnjm);
    }
}

std::string TermGenerator::UnRankT(size_t n, size_t m, int64_t k) {
    if (n == 0) {
        return std::to_string(k - 1);// k-1 cause notation begins with 0
    }

    if (k <= GetCount(n - 1, m + 1)) {
        return "(" + std::string("Abs") + " " + UnRankT(n - 1, m + 1, k) + ")";
    } else {
        return AppTerm(m, n - 1, 0, k - GetCount(n - 1, m + 1));
    }
}
