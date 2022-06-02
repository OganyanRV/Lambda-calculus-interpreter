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

public:
    AbstractSyntaxTree(const std::string expression) : expression_(expression) {
        root_ = std::make_shared<TermNode>(0, expression_.size() - 1, expression_);
        BuildTree(root_);
        CalculateDeBruijnNotation(root_);
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
                auto current_node = std::make_shared<App>();
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
                if ( var_it == name_context_.end()) {
                    name_context_.push_back(cur_var);
                    cur->SetDeBruijnIndex(bound_vars.size() + name_context_.size() - 1);
                } else {
                    size_t pos =  var_it - name_context_.begin();
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

    std::string ExprToString(const std::shared_ptr<TermNode> &from) {
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
};
#endif//OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
