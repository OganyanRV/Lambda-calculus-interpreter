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
        for (size_t free_var_idx = 0; free_var_idx <= kMaxFreeVarsCount; ++free_var_idx) {
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

std::string TermGenerator::AppTermStr(size_t max_free_var_cnt, size_t term_size, size_t j, int64_t h) {
    int64_t tnjm = GetCount(term_size - j, max_free_var_cnt);
    int64_t tjmtnjm = GetCount(j, max_free_var_cnt) * tnjm;

    if (h <= tjmtnjm) {
        int64_t dv = (h - 1) / tnjm;
        int64_t md = (h - 1) % tnjm;
        std::string left_term = UnRankTStr(j, max_free_var_cnt, dv + 1);
        std::string right_term = UnRankTStr(term_size - j, max_free_var_cnt, md + 1);
        return std::string("(App ") + left_term + " " + right_term + ")";
    } else {
        return AppTermStr(max_free_var_cnt, term_size, j + 1, h - tjmtnjm);
    }
}

std::string TermGenerator::UnRankTStr(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term) {
    if (term_size == 0) {
        return std::to_string(number_of_term - 1);// k-1 cause notation begins with 0
    }

    if (number_of_term <= GetCount(term_size - 1, max_free_var_cnt + 1)) {
        return "(Abs " + UnRankTStr(term_size - 1, max_free_var_cnt + 1, number_of_term) + ")";
    } else {
        return AppTermStr(max_free_var_cnt, term_size - 1, 0,
                          number_of_term - GetCount(term_size - 1, max_free_var_cnt + 1));
    }
}

std::string TermGenerator::GenerateTermStr(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term) {
    return UnRankTStr(term_size, max_free_var_cnt, number_of_term);
}


std::shared_ptr<TermNode> TermGenerator::AppTerm(size_t max_free_var_cnt, size_t term_size, size_t j,
                                                 int64_t h, std::shared_ptr<TermNode> from) {
    int64_t tnjm = GetCount(term_size - j, max_free_var_cnt);
    int64_t tjmtnjm = GetCount(j, max_free_var_cnt) * tnjm;

    if (h <= tjmtnjm) {
        int64_t dv = (h - 1) / tnjm;
        int64_t md = (h - 1) % tnjm;
        auto app = std::make_shared<App>();
        auto left_term = UnRankT(j, max_free_var_cnt, dv + 1, app);
        auto right_term = UnRankT(term_size - j, max_free_var_cnt, md + 1, app);

        app->SetLeft(left_term);
        app->SetRight(right_term);
        app->SetParent(from);
        left_term->SetChildType(ChildType::kLeft);
        right_term->SetChildType(ChildType::kRight);
        return app;
    } else {
        return AppTerm(max_free_var_cnt, term_size, j + 1, h - tjmtnjm, from);
    }
}
std::shared_ptr<TermNode> TermGenerator::UnRankT(size_t term_size, size_t max_free_var_cnt,
                                                 int64_t number_of_term, std::shared_ptr<TermNode> from) {
    if (term_size == 0) {
        auto var = std::make_shared<Var>(std::to_string(number_of_term - 1));
        var->SetDeBruijnIndex(number_of_term - 1);
        var->SetParent(from);
        return var;
    }

    if (number_of_term <= GetCount(term_size - 1, max_free_var_cnt + 1)) {
        auto abs = std::make_shared<Abs>("\\");
        abs->SetParent(from);

        auto sub_term = UnRankT(term_size - 1, max_free_var_cnt + 1,
                                number_of_term, abs);
        sub_term->SetParent(abs);
        abs->SetDown(sub_term);
        abs->SetChildType(ChildType::kDown);
        return abs;
    } else {
        return AppTerm(max_free_var_cnt, term_size - 1, 0,
                          number_of_term - GetCount(term_size - 1, max_free_var_cnt + 1), from);
    }
}

AbstractSyntaxTree TermGenerator::GenerateTerm(size_t term_size, size_t max_free_var_cnt, int64_t number_of_term) {
    std::shared_ptr<Abs> temp_root = {};
    std::shared_ptr<TermNode> root = UnRankT(term_size, max_free_var_cnt, number_of_term,
                                             std::static_pointer_cast<TermNode>(temp_root));
    return AbstractSyntaxTree(root);
}
