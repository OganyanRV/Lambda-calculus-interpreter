#ifndef OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#define OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
#include "TermNode.h"

class AbstractSyntaxTree {
private:
    std::shared_ptr<TermNode> root_;
    std::string expression_;
    //    std::vector<char> expression_vec;

    size_t FindClosingBracket(size_t begin_idx, size_t end_idx) {
        size_t cur_idx = begin_idx;
        int64_t brackets_count = 0;
        while(cur_idx <= end_idx) {
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

    std::pair<TermType, std::array<size_t, 4>> SplitIntoTerms(size_t begin_idx, size_t end_idx){
        std::array<size_t, 4> new_term_indexes;
        TermType new_term_type;

        size_t new_begin_idx = begin_idx;
        size_t new_end_idx = end_idx;

        size_t cur_index = new_begin_idx;
        while (true) {
            cur_index = new_begin_idx;
            while(expression_[cur_index] == ' ' && cur_index <= new_end_idx) {
                ++cur_index;
                ++new_begin_idx;
            }

            if (expression_[cur_index] == '(') {
                size_t bracket_end_idx = FindClosingBracket(new_begin_idx + 1, new_end_idx);
                if (expression_[cur_index + 1] == '\\') {
                    new_term_indexes[0] = cur_index + 1;
                    if (bracket_end_idx == new_end_idx) {
                        // (\x x c) case
                        new_term_indexes[1] = new_end_idx - 1;
                        new_term_type = TermType::kAbs;
                        return {new_term_type, new_term_indexes};
                    } else {
                        // (\x x c) a
                        new_term_type = TermType::kApp;
                        new_term_indexes[0] = cur_index;
                        new_term_indexes[1] = bracket_end_idx;
                        new_term_indexes[2] = bracket_end_idx +1;
                        new_term_indexes[3] = new_end_idx;
                        return {new_term_type, new_term_indexes};
                    }
                }
                else {
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
                    }
                    else {
                        // (a b) (\x x) case
                        new_term_type = TermType::kApp;
                        new_term_indexes[0] = cur_index;
                        new_term_indexes[1] = bracket_end_idx;
                        new_term_indexes[2] = bracket_end_idx +1;
                        new_term_indexes[3] = new_end_idx;
                        return {new_term_type, new_term_indexes};
                    }
                }
            }
            else {
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
                    new_term_indexes[2] = cur_index +1;
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
    }

    void BuildTree(std::shared_ptr<TermNode> &from) {
        // Im here
        auto terms { SplitIntoTerms(from->begin_idx_, from->end_idx_)};
        if (root_ == from) {
            if (terms.first == TermType::kVar) {
                auto current_node = std::make_shared<Var>(terms.second[0], terms.second[1]);
            }
            std::cout<<"KEK"<<std::endl;
        }

        if (terms.first == TermType::kVar) {
            auto current_node = std::make_shared<Var>(terms.second[0], terms.second[1]);
            if (root_ != from) {

            }
            if (from->parent_.lock()->type_ == TermType::kAbs) {
                auto parent = std::static_pointer_cast<Abs>(from->parent_.lock());
                current_node->parent_ = parent;
                parent->SetDown(current_node);
                return;
            }


        }
        else if (terms.first == TermType::kAbs) {

        }
        else if (terms.first == TermType::kApp) {

        }

    }
};
#endif//OGANYAN_LAMBDA_CALC_ABSTRACTSYNTAXTREE_H
