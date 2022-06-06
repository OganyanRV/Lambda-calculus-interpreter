#ifndef OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#define OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#include "TermNode.h"
#include <algorithm>

class AbstractSyntaxTree {
private:
    std::shared_ptr<TermNode> root_;
    std::string expression_;
    std::vector<char> name_context_;
    //    const static int8_t kAlphabetSize = 26;//  Need also for naming context and de bruijn notation
    //    std::vector<char> expression_vec;

    size_t FindClosingBracket(size_t begin_idx, size_t end_idx) {
        size_t cur_idx = begin_idx;
        int64_t brackets_count = 0;
        while (cur_idx <= end_idx) {
            if (this->expression_[cur_idx] == '(') {
                ++brackets_count;
            }
            if (this->expression_[cur_idx] == ')') {
                --brackets_count;
            }
            if (brackets_count == -1) {
                break;
            }
            ++cur_idx;
        }
        return cur_idx;
    }

    std::pair<TermType, std::array<size_t, 4>> SplitIntoTerms(size_t begin_idx, size_t end_idx) {
        std::array<size_t, 4> new_term_indexes;
        TermType new_term_type;

        size_t new_begin_idx = begin_idx;
        size_t new_end_idx = end_idx;

        size_t cur_index = new_begin_idx;
        while (true) {
            cur_index = new_begin_idx;
            while (expression_[cur_index] == ' ' && cur_index <= new_end_idx) {
                ++cur_index;
                ++new_begin_idx;
            }

            if (expression_[cur_index] == '(') {
                size_t bracket_end_idx = FindClosingBracket(new_begin_idx + 1, new_end_idx);
                if (expression_[cur_index + 1] == '\\') {
                    if (bracket_end_idx == new_end_idx) {
                        // (\x x c) case
                        new_term_indexes[0] = cur_index + 1;
                        new_term_indexes[1] = cur_index + 2;
                        new_term_indexes[2] = cur_index + 3;
                        new_term_indexes[3] = new_end_idx - 1;
                        new_term_type = TermType::kAbs;
                        return {new_term_type, new_term_indexes};
                    } else {
                        // (\x x c) a
                        new_term_type = TermType::kApp;
                        new_term_indexes[0] = cur_index;
                        new_term_indexes[1] = bracket_end_idx;
                        new_term_indexes[2] = bracket_end_idx + 1;
                        new_term_indexes[3] = new_end_idx;
                        return {new_term_type, new_term_indexes};
                    }
                } else {
                    if (bracket_end_idx == new_end_idx) {
                        // ((\x x) a) case
                        ++new_begin_idx;
                        --new_end_idx;
                    } else if (bracket_end_idx == cur_index + 2) {
                        // (x) case
                        new_term_type = TermType::kVar;
                        new_term_indexes[0] = cur_index + 1;
                        new_term_indexes[1] = cur_index + 1;
                        return {new_term_type, new_term_indexes};
                    } else {
                        // (a b) (\x x) case
                        new_term_type = TermType::kApp;
                        new_term_indexes[0] = cur_index;
                        new_term_indexes[1] = bracket_end_idx;
                        new_term_indexes[2] = bracket_end_idx + 1;
                        new_term_indexes[3] = new_end_idx;
                        return {new_term_type, new_term_indexes};
                    }
                }
            } else {
                if (cur_index == new_end_idx || cur_index == new_end_idx + 1) {
                    // x case
                    new_term_type = TermType::kVar;
                    new_term_indexes[0] = cur_index;
                    new_term_indexes[1] = cur_index;
                    return {new_term_type, new_term_indexes};
                } else {
                    // x (\y y)
                    new_term_type = TermType::kApp;
                    new_term_indexes[0] = cur_index;
                    new_term_indexes[1] = cur_index;
                    new_term_indexes[2] = cur_index + 1;
                    new_term_indexes[3] = new_end_idx;
                    return {new_term_type, new_term_indexes};
                }
            }
        }
    }

    void BuildTree(std::shared_ptr<TermNode> &from) {
        auto terms{SplitIntoTerms(from->begin_idx_, from->end_idx_)};
        if (root_ == from) {
            if (terms.first == TermType::kVar) {
                auto current_node = std::make_shared<Var>(terms.second[0], terms.second[1], expression_);
                root_ = current_node;
                return;
            }
            if (terms.first == TermType::kAbs) {
                auto current_node = std::make_shared<Abs>(terms.second[0], terms.second[1], expression_);
                auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                           terms.second[2], terms.second[3], expression_);
                current_node->SetDown(sub_term);
                sub_term->SetParent(current_node);
                root_ = current_node;
                return BuildTree(sub_term);
            }
            if (terms.first == TermType::kApp) {
                auto current_node = std::make_shared<App>();
                auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                           terms.second[0], terms.second[1], expression_);
                auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                            terms.second[2], terms.second[3], expression_);
                left_app->parent_ = current_node;
                right_app->parent_ = current_node;
                current_node->SetLeft(left_app);
                current_node->SetRight(right_app);
                root_ = current_node;
                BuildTree(left_app);
                BuildTree(right_app);
            }
        } else {
            if (terms.first == TermType::kVar) {
                auto current_node = std::make_shared<Var>(from->child_type_,
                                                          terms.second[0], terms.second[1], expression_);
                current_node->SetParent(from->GetParent());
                if (from->child_type_ == ChildType::kDown) {
                    auto parent = std::static_pointer_cast<Abs>(from->parent_.lock());
                    parent->SetDown(current_node);
                } else if (from->child_type_ == ChildType::kLeft) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetLeft(current_node);
                } else if (from->child_type_ == ChildType::kRight) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetRight(current_node);
                }
                return;
            }
            if (terms.first == TermType::kAbs) {
                auto current_node = std::make_shared<Abs>(from->child_type_,
                                                          terms.second[0], terms.second[1], expression_);
                auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                           terms.second[2], terms.second[3], expression_);
                current_node->SetDown(sub_term);
                current_node->SetParent(from->GetParent());
                sub_term->SetParent(current_node);
                if (from->child_type_ == ChildType::kDown) {
                    auto parent = std::static_pointer_cast<Abs>(from->parent_.lock());
                    parent->SetDown(current_node);
                } else if (from->child_type_ == ChildType::kLeft) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetLeft(current_node);
                } else if (from->child_type_ == ChildType::kRight) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetRight(current_node);
                }
                return BuildTree(sub_term);
            }
            if (terms.first == TermType::kApp) {
                auto current_node = std::make_shared<App>(from->child_type_);
                auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                           terms.second[0], terms.second[1], expression_);
                auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                            terms.second[2], terms.second[3], expression_);
                left_app->parent_ = current_node;
                right_app->parent_ = current_node;
                current_node->SetLeft(left_app);
                current_node->SetRight(right_app);
                current_node->SetParent(from->GetParent());

                if (from->child_type_ == ChildType::kDown) {
                    auto parent = std::static_pointer_cast<Abs>(from->parent_.lock());
                    parent->SetDown(current_node);
                } else if (from->child_type_ == ChildType::kLeft) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetLeft(current_node);
                } else if (from->child_type_ == ChildType::kRight) {
                    auto parent = std::static_pointer_cast<App>(from->parent_.lock());
                    parent->SetRight(current_node);
                }
                BuildTree(left_app);
                BuildTree(right_app);
            }
        }
    }

    void CalculateDeBruijnNotation(const std::shared_ptr<TermNode> &from, std::vector<char> bound_vars = {}) {
        if (from->type_ == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            CalculateDeBruijnNotation(cur->GetLeft(), bound_vars);
            CalculateDeBruijnNotation(cur->GetRight(), bound_vars);
        } else if (from->type_ == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(from);
            bound_vars.push_back(this->expression_[cur->begin_idx_ + 1]);
            CalculateDeBruijnNotation(cur->GetDown(), bound_vars);
        } else if (from->type_ == TermType::kVar) {
            auto cur = std::static_pointer_cast<Var>(from);
            char cur_var = this->expression_[cur->begin_idx_];

            auto find = [](const std::vector<char> &bound_vars, char var) -> int64_t {
                for (int64_t idx = bound_vars.size() - 1; idx >= 0; --idx) {
                    if (bound_vars[static_cast<size_t>(idx)] == var) {
                        return idx;
                    }
                }
                return -1;
            };

            auto shift = find(bound_vars, cur_var);
            if (shift == -1) {
                //  free var
                auto var_it = std::find(name_context_.begin(), name_context_.end(), cur_var);
                if (var_it == name_context_.end()) {
                    name_context_.push_back(cur_var);
                    cur->SetDeBruijnIndex(bound_vars.size() + name_context_.size() - 1);
                } else {
                    size_t pos = var_it - name_context_.begin();
                    cur->SetDeBruijnIndex(bound_vars.size() + pos);
                }
                cur->SetIsFree(true);
            } else {
                //  bound var
                cur->SetDeBruijnIndex(bound_vars.size() - shift - 1);
                cur->SetIsFree(false);
            }
        }
    }

    void Shift(std::shared_ptr<TermNode> &from, int64_t d_pos, int64_t cutoff) {
        if (from->type_ == TermType::kVar) {
            auto cur = std::static_pointer_cast<Var>(from);
            if (cur->GetDeBruijnIndex() >= cutoff) {
                cur->SetDeBruijnIndex(cur->GetDeBruijnIndex() + d_pos);
            }
        } else if (from->type_ == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(from);
            Shift(cur->GetDown(), d_pos, cutoff + 1);

        } else if (from->type_ == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            Shift(cur->GetLeft(), d_pos, cutoff);
            Shift(cur->GetRight(), d_pos, cutoff);
        }
    }

    void Substitution(std::shared_ptr<TermNode> &term_to_reduce, std::shared_ptr<TermNode> &value, size_t j) {
        if (term_to_reduce->type_ == TermType::kVar) {
            auto cur = std::static_pointer_cast<Var>(term_to_reduce);
            if (cur->GetDeBruijnIndex() == j) {
                auto parent = cur->GetParent().lock();
                auto new_value = CopySubTree(value);
                if (parent->type_ == TermType::kAbs) {
                    auto parent_downcast = std::static_pointer_cast<Abs>(parent);
                    new_value->SetParent(parent_downcast);
                    parent_downcast->SetDown(new_value);
                } else if (parent->type_ == TermType::kApp) {
                    auto parent_downcast = std::static_pointer_cast<App>(parent);
                    new_value->SetParent(parent_downcast);
                    if (cur->GetChildType() == ChildType::kLeft) {
                        parent_downcast->SetLeft(new_value);
                    } else if (cur->GetChildType() == ChildType::kRight) {
                        parent_downcast->SetRight(new_value);
                    }
                }
            }
        } else if (term_to_reduce->type_ == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(term_to_reduce);
            auto new_value = CopySubTree(value);
            Shift(new_value, 1, 0);
            Substitution(cur->GetDown(), new_value, j + 1);
        } else if (term_to_reduce->type_ == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(term_to_reduce);
            Substitution(cur->GetLeft(), value, j);
            Substitution(cur->GetRight(), value, j);
        }
    }

    std::shared_ptr<TermNode> CopySubTree(const std::shared_ptr<TermNode> &from) const {
        std::shared_ptr<TermNode> copied_sub_tree = std::shared_ptr<TermNode>(from);
        CopySubTreeRecursive(copied_sub_tree, from);
        return copied_sub_tree;
    }

    void CopySubTreeRecursive(std::shared_ptr<TermNode> &copy_node, const std::shared_ptr<TermNode> &from) const {
        if (from->type_ == TermType::kVar) {
            auto parent = copy_node->GetParent();
            copy_node = std::make_shared<Var>(from->GetChildType(), from->GetBeginIdx(),
                                              from->GetEndIdx(), this->expression_);
            auto cur_copy_node = std::static_pointer_cast<Var>(copy_node);
            auto cur_source_node = std::static_pointer_cast<Var>(from);
            cur_copy_node->SetParent(parent);
            cur_copy_node->SetIsFree(cur_source_node->IsFree());
            cur_copy_node->SetDeBruijnIndex(cur_source_node->GetDeBruijnIndex());
        } else if (from->type_ == TermType::kAbs) {
            auto parent = copy_node->GetParent();
            copy_node = std::make_shared<Abs>(from->GetChildType(), from->GetBeginIdx(),
                                              from->GetEndIdx(), this->expression_);
            auto cur_copy_node = std::static_pointer_cast<Abs>(copy_node);
            auto cur_source_node = std::static_pointer_cast<Abs>(from);

            cur_copy_node->SetParent(parent);
            cur_copy_node->SetDown(std::shared_ptr<TermNode>(cur_source_node->GetDown()));
            cur_copy_node->GetDown()->SetParent(cur_copy_node);
            CopySubTreeRecursive(cur_copy_node->GetDown(), cur_source_node->GetDown());
        } else if (from->type_ == TermType::kApp) {
            auto parent = copy_node->GetParent();
            copy_node = std::make_shared<App>(from->GetChildType(), from->GetBeginIdx(),
                                              from->GetEndIdx(), this->expression_);
            auto cur_copy_node = std::static_pointer_cast<App>(copy_node);
            auto cur_source_node = std::static_pointer_cast<App>(from);

            cur_copy_node->SetParent(parent);
            cur_copy_node->SetLeft(std::shared_ptr<TermNode>(cur_source_node->GetLeft()));
            cur_copy_node->SetRight(std::shared_ptr<TermNode>(cur_source_node->GetRight()));
            cur_copy_node->GetLeft()->SetParent(cur_copy_node);
            cur_copy_node->GetRight()->SetParent(cur_copy_node);
            CopySubTreeRecursive(cur_copy_node->GetLeft(), cur_source_node->GetLeft());
            CopySubTreeRecursive(cur_copy_node->GetRight(), cur_source_node->GetRight());
        }
    }

    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInApplicativeStrategy(const std::shared_ptr<TermNode> &from) {
        if (from->GetType() == TermType::kAbs) {
            return {false, {}};
        }
        if (from->GetType() == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            auto left_term = cur->GetLeft();
            auto right_term = cur->GetRight();
            if (left_term->GetType() == TermType::kAbs) {
                auto res_for_right = FindRedexInApplicativeStrategy(right_term);
                if (!res_for_right.first) {
                    return {true, cur};
                } else {
                    return res_for_right;
                }
            } else {
                return FindRedexInApplicativeStrategy(left_term);
            }
        }
        return {false, {}};
    }

    //    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInApplicativeStrategy(const std::shared_ptr<TermNode> &from) {
    //        if (from->GetType() == TermType::kAbs) {
    //            return {false, {}};
    //        }
    //        if (from->GetType() == TermType::kApp) {
    //            auto cur = std::static_pointer_cast<App>(from);
    //            auto left_term = cur->GetLeft();
    //            auto right_term = cur->GetRight();
    //            if (left_term->GetType() == TermType::kAbs) {
    //                if (right_term->GetType() != TermType::kAbs) {
    //                    return FindRedexInApplicativeStrategy(right_term);
    //                } else {
    //                    return {true, cur};
    //                }
    //            }
    //        }
    //        return {false, {}};
    //    }

    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInLazyStrategy(const std::shared_ptr<TermNode> &from) {
        if (from->GetType() == TermType::kAbs) {
            return {false, {}};
        }
        if (from->GetType() == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            auto left_term = cur->GetLeft();
            auto right_term = cur->GetRight();
            if (left_term->GetType() == TermType::kAbs) {
                return {true, cur};
            } else {
                return FindRedexInLazyStrategy(left_term);
            }
        }
        return {false, {}};
    }

    //    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInLazyStrategy(const std::shared_ptr<TermNode> &from) {
    //        if (from->GetType() == TermType::kApp) {
    //            auto cur = std::static_pointer_cast<App>(from);
    //            auto left_term = cur->GetLeft();
    //            auto right_term = cur->GetRight();
    //            if (left_term->GetType() == TermType::kAbs) {
    //                return {true, cur};
    //            } else {
    //                return FindRedexInLazyStrategy(right_term);
    //            }
    //        }
    //        return {false, {}};
    //    }

    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInNormalStrategy(const std::shared_ptr<TermNode> &from) {
        if (from->GetType() == TermType::kVar) {
            return {false, {}};
        }
        if (from->GetType() == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(from);
            return FindRedexInNormalStrategy(cur->GetDown());
        }
        if (from->GetType() == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            auto left_term = cur->GetLeft();
            auto right_term = cur->GetRight();
            auto res_for_left = FindRedexInNormalStrategy(left_term);
            if (res_for_left.first) {
                return res_for_left;
            } else {
                if (left_term->GetType() == TermType::kAbs) {
                    return {true, cur};
                } else {
                    return FindRedexInNormalStrategy(right_term);
                }
            }
        }
        return {false, {}};
    }
    //    std::pair<bool, std::shared_ptr<TermNode>> FindRedexInNormalStrategy(const std::shared_ptr<TermNode> &from) {
    //        if (from->GetType() == TermType::kApp) {
    //            auto cur = std::static_pointer_cast<App>(from);
    //            auto left_term = cur->GetLeft();
    //            auto right_term = cur->GetRight();
    //            if (left_term->GetType() == TermType::kAbs) {
    //                return {true, cur};
    //            } else {
    //                auto res_for_subterm = FindRedexInNormalStrategy(std::static_pointer_cast<Abs>(left_term)->GetDown());
    //                if (res_for_subterm.first) {
    //                    return {true, res_for_subterm.second};
    //                } else {
    //                    return FindRedexInNormalStrategy(right_term);
    //                }
    //            }
    //        } else if (from->GetType() == TermType::kAbs) {
    //            return FindRedexInNormalStrategy(std::static_pointer_cast<Abs>(from)->GetDown());
    //        }
    //        return {false, {}};
    //    }

public:
    AbstractSyntaxTree(const std::string expression) : expression_(expression) {
        root_ = std::make_shared<TermNode>(0, expression_.size() - 1, expression_);
        BuildTree(root_);
        CalculateDeBruijnNotation(root_);
    }

    AbstractSyntaxTree(const AbstractSyntaxTree &other) : expression_(other.expression_), name_context_(other.name_context_) {
        root_ = other.CopySubTree(other.root_);
    }

    AbstractSyntaxTree &operator=(const AbstractSyntaxTree &other) {
        expression_ = other.expression_;
        name_context_ = other.name_context_;
        root_ = other.CopySubTree(other.root_);
    }

    virtual ~AbstractSyntaxTree() = default;


    std::vector<std::string> ApplicativeBetaReduction() {
        std::vector<std::string> reduction_steps;
        reduction_steps.push_back(ExprToStringDB(this->root_));
        while (true) {
            auto redex = FindRedexInApplicativeStrategy(this->root_);
            if (!redex.first) {
                break;
            }
            MakeReductionStep(redex.second);
            reduction_steps.push_back(ExprToStringDB(this->root_));

            if (reduction_steps[reduction_steps.size() - 1] == reduction_steps[reduction_steps.size() - 2]) {
                reduction_steps.pop_back();
                break;
            }
        }
        return reduction_steps;
    }


    std::vector<std::string> LazyBetaReduction() {
        std::vector<std::string> reduction_steps;
        reduction_steps.push_back(ExprToStringDB(this->root_));
        while (true) {
            auto redex = FindRedexInLazyStrategy(this->root_);
            if (!redex.first) {
                break;
            }
            MakeReductionStep(redex.second);
            reduction_steps.push_back(ExprToStringDB(this->root_));

            if (reduction_steps[reduction_steps.size() - 1] == reduction_steps[reduction_steps.size() - 2]) {
                reduction_steps.pop_back();
                break;
            }
        }
        return reduction_steps;
    }


    std::vector<std::string> NormalBetaReduction() {
        std::vector<std::string> reduction_steps;
        reduction_steps.push_back(ExprToStringDB(this->root_));
        while (true) {
            auto redex = FindRedexInNormalStrategy(this->root_);
            if (!redex.first) {
                break;
            }
            MakeReductionStep(redex.second);
            reduction_steps.push_back(ExprToStringDB(this->root_));

            if (reduction_steps[reduction_steps.size() - 1] == reduction_steps[reduction_steps.size() - 2]) {
                reduction_steps.pop_back();
                break;
            }
        }
        return reduction_steps;
    }

    std::string
    ExprToString(const std::shared_ptr<TermNode> &from) {
        if (from->type_ == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            auto left_str = ExprToString(cur->GetLeft());
            auto right_str = ExprToString(cur->GetRight());
            return "(" + left_str + " " + right_str + ")";
        } else if (from->type_ == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(from);
            auto down_str = ExprToString(cur->GetDown());
            return "(" + cur->GetTerm() + " " + "(" + down_str + ")" + ")";
        } else if (from->type_ == TermType::kVar) {
            auto cur = std::static_pointer_cast<Var>(from);
            return cur->GetTerm();
        }
        return {};
    }

    std::string ExprToStringDB(const std::shared_ptr<TermNode> &from) {
        if (from->type_ == TermType::kApp) {
            auto cur = std::static_pointer_cast<App>(from);
            auto left_str = ExprToStringDB(cur->GetLeft());
            auto right_str = ExprToStringDB(cur->GetRight());
            return "(" + left_str + " " + right_str + ")";
        } else if (from->type_ == TermType::kAbs) {
            auto cur = std::static_pointer_cast<Abs>(from);
            auto down_str = ExprToStringDB(cur->GetDown());
            return "(" + std::string("\\") + std::string(" ") + "(" + down_str + ")" + ")";
        } else if (from->type_ == TermType::kVar) {
            auto cur = std::static_pointer_cast<Var>(from);
            return std::to_string(cur->GetDeBruijnIndex());
        }
        return {};
    }

    const std::shared_ptr<TermNode> &GetRoot() const {
        return root_;
    }

    std::shared_ptr<TermNode> &GetRoot() {
        return root_;
    }
    void MakeReductionStep(std::shared_ptr<TermNode> &from) {
        auto cur = std::static_pointer_cast<App>(from);
        auto term_to_reduce = std::static_pointer_cast<Abs>(cur->GetLeft());
        auto value = cur->GetRight();
        Shift(value, 1, 0);
        Substitution(term_to_reduce->GetDown(), value, 0);
        auto new_reduced_node = std::static_pointer_cast<Abs>(term_to_reduce->GetDown()->GetParent().lock())->GetDown();
        Shift(new_reduced_node, -1, 0);
        if (from->GetParent().expired()) {
            root_ = new_reduced_node;
            root_->SetParent({});
        } else {
            if (from->GetParent().lock()->type_ == TermType::kAbs) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(new_reduced_node);
                new_reduced_node->SetParent(parent);
            } else if (from->GetParent().lock()->type_ == TermType::kApp) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                new_reduced_node->SetParent(parent);
                if (from->child_type_ == ChildType::kLeft) {
                    parent->SetLeft(new_reduced_node);
                } else if (from->child_type_ == ChildType::kRight) {
                    parent->SetRight(new_reduced_node);
                }
            }
        }
    }
};


#endif//OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
