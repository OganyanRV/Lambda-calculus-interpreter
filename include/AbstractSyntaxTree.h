#ifndef OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#define OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#include "TermNode.h"
#include <algorithm>
#include <string>
#include <vector>

class AbstractSyntaxTree {
private:
    std::shared_ptr<TermNode> root_;
    std::string source_exp_;
    std::vector<char> name_context_;

    size_t FindClosingBracket(size_t begin_idx, size_t end_idx);
    std::pair<TermType, std::array<size_t, 4>> SplitIntoTerms(size_t begin_idx, size_t end_idx);
    void BuildTreeNormalStyle(std::shared_ptr<TermNode> &from, size_t begin_idx, size_t end_idx);
    void CalculateDeBruijnNotation(const std::shared_ptr<TermNode> &from, std::vector<char> bound_vars = {});

    std::pair<TermType, std::array<size_t, 4>> SplitIntoTermsHaskellStyle(size_t begin_idx, size_t end_idx);
    void BuildTreeHaskellStyle(std::shared_ptr<TermNode> &from, size_t begin_idx, size_t end_idx);

    void Shift(std::shared_ptr<TermNode> &from, int64_t d_pos, int64_t cutoff);
    void Substitution(std::shared_ptr<TermNode> &term_to_reduce, std::shared_ptr<TermNode> &value, size_t j);
    void MakeReductionStep(std::shared_ptr<TermNode> &from);

    std::shared_ptr<TermNode> CopySubTree(const std::shared_ptr<TermNode> &from) const;
    void CopySubTreeRecursive(std::shared_ptr<TermNode> &copy_node, const std::shared_ptr<TermNode> &from) const;

    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInCallByValue(const std::shared_ptr<TermNode> &from);
    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInCallByName(const std::shared_ptr<TermNode> &from);
    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInNormalStrategy(const std::shared_ptr<TermNode> &from);

public:
    AbstractSyntaxTree(const std::string &expression, InputType input_type = InputType::kNormal);
    AbstractSyntaxTree(const std::shared_ptr<TermNode> &root);
    AbstractSyntaxTree(const AbstractSyntaxTree &other);
    AbstractSyntaxTree &operator=(const AbstractSyntaxTree &other);
    ~AbstractSyntaxTree() = default;

    std::pair<NormalFormType, std::pair<std::vector<std::string>,
                                        std::vector<int64_t>>>
    BetaReduction(StrategyType strategy_type, size_t steps_limit = 200, size_t term_size_limit = 8048);

    std::string ExprToStringDB(const std::shared_ptr<TermNode> &from);
    std::string ExprToStringHaskell(const std::shared_ptr<TermNode> &from);

    int64_t CalculateTermSize(const std::shared_ptr<TermNode> &from);

    const std::shared_ptr<TermNode> &GetRoot() const;
    std::shared_ptr<TermNode> &GetRoot();
};

#endif//OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
