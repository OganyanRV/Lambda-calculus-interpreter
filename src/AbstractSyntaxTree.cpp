#include "../include/AbstractSyntaxTree.h"
#include "../include/Syntax_functions.h"

AbstractSyntaxTree::AbstractSyntaxTree(const std::string &expression, InputType input_type) : source_exp_(expression) {
    source_exp_ = MakeCorrectForm(source_exp_, input_type);
    if (input_type == InputType::kNormal) {
        root_ = std::make_shared<TermNode>(source_exp_);
        BuildTreeNormalStyle(root_, 0, source_exp_.size() - 1);
        CalculateDeBruijnNotation(root_);
    } else if (input_type == InputType::kHaskell) {
        root_ = std::make_shared<TermNode>(source_exp_);
        BuildTreeHaskellStyle(root_, 0, source_exp_.size() - 1);
    }
}

AbstractSyntaxTree::AbstractSyntaxTree(const std::shared_ptr<TermNode> &root) {
    root_ = root;
    source_exp_ = ExprToStringHaskell(root_);
}

AbstractSyntaxTree::AbstractSyntaxTree(const AbstractSyntaxTree &other) : source_exp_(other.source_exp_),
                                                                          name_context_(other.name_context_) {
    root_ = other.CopySubTree(other.root_);
}


AbstractSyntaxTree &AbstractSyntaxTree::operator=(const AbstractSyntaxTree &other) {
    source_exp_ = other.source_exp_;
    name_context_ = other.name_context_;
    root_ = other.CopySubTree(other.root_);
    return *this;
}

size_t AbstractSyntaxTree::FindClosingBracket(size_t begin_idx, size_t end_idx) {
    size_t cur_idx = begin_idx;
    int64_t brackets_count = 0;
    while (cur_idx <= end_idx) {
        if (this->source_exp_[cur_idx] == '(') {
            ++brackets_count;
        }
        if (this->source_exp_[cur_idx] == ')') {
            --brackets_count;
        }
        if (brackets_count == -1) {
            break;
        }
        ++cur_idx;
    }
    return cur_idx;
}

std::pair<TermType, std::array<size_t, 4>> AbstractSyntaxTree::SplitIntoTerms(size_t begin_idx, size_t end_idx) {
    std::array<size_t, 4> new_term_indexes;
    TermType new_term_type;

    size_t new_begin_idx = begin_idx;
    size_t new_end_idx = end_idx;

    size_t cur_index;
    while (true) {
        cur_index = new_begin_idx;
        while (source_exp_[cur_index] == ' ' && cur_index <= new_end_idx) {
            ++cur_index;
            ++new_begin_idx;
        }

        if (source_exp_[cur_index] == '(') {
            size_t bracket_end_idx = FindClosingBracket(new_begin_idx + 1, new_end_idx);
            if (source_exp_[cur_index + 1] == '\\') {
                if (bracket_end_idx == new_end_idx) {
                    // (\x x c) case
                    new_term_indexes[0] = cur_index + 1;
                    new_term_indexes[1] = cur_index + 2;
                    new_term_indexes[2] = cur_index + 3;
                    new_term_indexes[3] = new_end_idx - 1;
                    new_term_type = TermType::kAbs;
                    return {new_term_type, new_term_indexes};
                } else {
                    // (\x x c) a case
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

std::pair<TermType, std::array<size_t, 4>> AbstractSyntaxTree::SplitIntoTermsHaskellStyle(size_t begin_idx, size_t end_idx) {
    std::array<size_t, 4> new_term_indexes;
    TermType new_term_type;

    size_t new_begin_idx = begin_idx;
    size_t new_end_idx = end_idx;

    size_t cur_index;
    cur_index = new_begin_idx;
    while ((source_exp_[cur_index] == ' ' || source_exp_[cur_index] == '(') && cur_index <= new_end_idx) {
        ++cur_index;
        ++new_begin_idx;
    }

    if (std::isdigit(source_exp_[cur_index])) {
        new_begin_idx = cur_index;
        while ((cur_index <= new_end_idx) && std::isdigit(source_exp_[cur_index])) {
            ++cur_index;
        }
        new_end_idx = cur_index - 1;
        new_term_type = TermType::kVar;
        new_term_indexes[0] = new_begin_idx;
        new_term_indexes[1] = new_end_idx;
        return {new_term_type, new_term_indexes};
    }

    if (std::isalpha(source_exp_[cur_index])) {
        std::string type_str{source_exp_[cur_index++]};
        while ((cur_index <= new_end_idx) && std::isalpha(source_exp_[cur_index])) {
            type_str += source_exp_[cur_index];
            ++cur_index;
        }
        if (type_str == "Abs") {
            while ((cur_index <= new_end_idx) && source_exp_[cur_index] != '(') {
                ++cur_index;
            }
            new_end_idx = FindClosingBracket(cur_index + 1, end_idx);
            new_term_type = TermType::kAbs;
            new_term_indexes[2] = cur_index + 1;
            new_term_indexes[3] = new_end_idx - 1;
            return {new_term_type, new_term_indexes};
        } else if (type_str == "App") {
            while ((cur_index <= new_end_idx) && source_exp_[cur_index] != '(') {
                ++cur_index;
            }
            new_end_idx = FindClosingBracket(cur_index + 1, end_idx);
            new_term_type = TermType::kApp;
            new_term_indexes[0] = cur_index + 1;
            new_term_indexes[1] = new_end_idx - 1;
            cur_index = new_end_idx + 1;
            while ((cur_index <= new_end_idx) && source_exp_[cur_index] != '(') {
                ++cur_index;
            }
            new_end_idx = FindClosingBracket(cur_index + 1, end_idx);
            new_term_indexes[2] = cur_index + 1;
            new_term_indexes[3] = new_end_idx - 1;
            return {new_term_type, new_term_indexes};
        }
    }
    return {new_term_type, new_term_indexes};
}

void AbstractSyntaxTree::BuildTreeNormalStyle(std::shared_ptr<TermNode> &from, size_t begin_idx, size_t end_idx) {
    auto terms{SplitIntoTerms(begin_idx, end_idx)};
    if (root_ == from) {
        if (terms.first == TermType::kVar) {
            auto current_node = std::make_shared<Var>(source_exp_.substr(terms.second[0],
                                                                         terms.second[1] - terms.second[0] + 1));
            root_ = current_node;
            return;
        }
        if (terms.first == TermType::kAbs) {
            auto current_node = std::make_shared<Abs>(source_exp_.substr(terms.second[0],
                                                                         terms.second[1] - terms.second[0] + 1));
            auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                       source_exp_.substr(terms.second[2],
                                                                          terms.second[2] - terms.second[3] + 1));
            current_node->SetDown(sub_term);
            sub_term->SetParent(current_node);
            root_ = current_node;
            return BuildTreeNormalStyle(sub_term, terms.second[2], terms.second[3]);
        }
        if (terms.first == TermType::kApp) {
            auto current_node = std::make_shared<App>();
            auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                       source_exp_.substr(terms.second[0],
                                                                          terms.second[1] - terms.second[0] + 1));
            auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                        source_exp_.substr(terms.second[2],
                                                                           terms.second[2] - terms.second[3] + 1));
            left_app->SetParent(current_node);
            right_app->SetParent(current_node);
            current_node->SetLeft(left_app);
            current_node->SetRight(right_app);
            root_ = current_node;
            BuildTreeNormalStyle(left_app, terms.second[0], terms.second[1]);
            BuildTreeNormalStyle(right_app, terms.second[2], terms.second[3]);
        }
    } else {
        if (terms.first == TermType::kVar) {
            auto current_node = std::make_shared<Var>(from->GetChildType(), source_exp_.substr(terms.second[0],
                                                                                               terms.second[1] - terms.second[0] + 1));
            current_node->SetParent(from->GetParent());
            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            return;
        }
        if (terms.first == TermType::kAbs) {
            auto current_node = std::make_shared<Abs>(from->GetChildType(), source_exp_.substr(terms.second[0],
                                                                                               terms.second[1] - terms.second[0] + 1));
            auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                       source_exp_.substr(terms.second[2],
                                                                          terms.second[2] - terms.second[3] + 1));
            current_node->SetDown(sub_term);
            current_node->SetParent(from->GetParent());
            sub_term->SetParent(current_node);
            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            return BuildTreeNormalStyle(sub_term, terms.second[2], terms.second[3]);
        }
        if (terms.first == TermType::kApp) {
            auto current_node = std::make_shared<App>(from->GetChildType());
            auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                       source_exp_.substr(terms.second[0],
                                                                          terms.second[1] - terms.second[0] + 1));
            auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                        source_exp_.substr(terms.second[2],
                                                                           terms.second[2] - terms.second[3] + 1));
            left_app->SetParent(current_node);
            right_app->SetParent(current_node);
            current_node->SetLeft(left_app);
            current_node->SetRight(right_app);
            current_node->SetParent(from->GetParent());

            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            BuildTreeNormalStyle(left_app, terms.second[0], terms.second[1]);
            BuildTreeNormalStyle(right_app, terms.second[2], terms.second[3]);
        }
    }
}

void AbstractSyntaxTree::BuildTreeHaskellStyle(std::shared_ptr<TermNode> &from, size_t begin_idx, size_t end_idx) {
    auto terms{SplitIntoTermsHaskellStyle(begin_idx, end_idx)};
    if (root_ == from) {
        if (terms.first == TermType::kVar) {
            auto current_node = std::make_shared<Var>(source_exp_.substr(terms.second[0],
                                                                         terms.second[1] - terms.second[0] + 1));
            current_node->SetDeBruijnIndex(std::stoi(current_node->GetTerm()));
            root_ = current_node;
            return;
        }
        if (terms.first == TermType::kAbs) {
            auto current_node = std::make_shared<Abs>();
            auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                       source_exp_.substr(terms.second[2],
                                                                          terms.second[2] - terms.second[3] + 1));
            current_node->SetDown(sub_term);
            sub_term->SetParent(current_node);
            root_ = current_node;
            return BuildTreeHaskellStyle(sub_term, terms.second[2], terms.second[3]);
        }
        if (terms.first == TermType::kApp) {
            auto current_node = std::make_shared<App>();
            auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                       source_exp_.substr(terms.second[0],
                                                                          terms.second[1] - terms.second[0] + 1));
            auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                        source_exp_.substr(terms.second[2],
                                                                           terms.second[2] - terms.second[3] + 1));
            left_app->SetParent(current_node);
            right_app->SetParent(current_node);
            current_node->SetLeft(left_app);
            current_node->SetRight(right_app);
            root_ = current_node;
            BuildTreeHaskellStyle(left_app, terms.second[0], terms.second[1]);
            BuildTreeHaskellStyle(right_app, terms.second[2], terms.second[3]);
        }
    } else {
        if (terms.first == TermType::kVar) {
            auto current_node = std::make_shared<Var>(from->GetChildType(), source_exp_.substr(terms.second[0],
                                                                                               terms.second[1] - terms.second[0] + 1));
            current_node->SetDeBruijnIndex(std::stoi(current_node->GetTerm()));
            current_node->SetParent(from->GetParent());
            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            return;
        }
        if (terms.first == TermType::kAbs) {
            auto current_node = std::make_shared<Abs>(from->GetChildType());
            auto sub_term = std::make_shared<TermNode>(ChildType::kDown,
                                                       source_exp_.substr(terms.second[2],
                                                                          terms.second[2] - terms.second[3] + 1));
            current_node->SetDown(sub_term);
            current_node->SetParent(from->GetParent());
            sub_term->SetParent(current_node);
            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            return BuildTreeHaskellStyle(sub_term, terms.second[2], terms.second[3]);
        }
        if (terms.first == TermType::kApp) {
            auto current_node = std::make_shared<App>(from->GetChildType());
            auto left_app = std::make_shared<TermNode>(ChildType::kLeft,
                                                       source_exp_.substr(terms.second[0],
                                                                          terms.second[1] - terms.second[0] + 1));
            auto right_app = std::make_shared<TermNode>(ChildType::kRight,
                                                        source_exp_.substr(terms.second[2],
                                                                           terms.second[2] - terms.second[3] + 1));
            left_app->SetParent(current_node);
            right_app->SetParent(current_node);
            current_node->SetLeft(left_app);
            current_node->SetRight(right_app);
            current_node->SetParent(from->GetParent());

            if (from->GetChildType() == ChildType::kDown) {
                auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
                parent->SetDown(current_node);
            } else if (from->GetChildType() == ChildType::kLeft) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetLeft(current_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
                parent->SetRight(current_node);
            }
            BuildTreeHaskellStyle(left_app, terms.second[0], terms.second[1]);
            BuildTreeHaskellStyle(right_app, terms.second[2], terms.second[3]);
        }
    }
}

void AbstractSyntaxTree::CalculateDeBruijnNotation(const std::shared_ptr<TermNode> &from, std::vector<char> bound_vars) {
    if (from->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(from);
        CalculateDeBruijnNotation(cur->GetLeft(), bound_vars);
        CalculateDeBruijnNotation(cur->GetRight(), bound_vars);
    } else if (from->GetType() == TermType::kAbs) {
        auto cur = std::static_pointer_cast<Abs>(from);
        bound_vars.push_back(cur->GetTerm()[1]);
        CalculateDeBruijnNotation(cur->GetDown(), bound_vars);
    } else if (from->GetType() == TermType::kVar) {
        auto cur = std::static_pointer_cast<Var>(from);
        char cur_var = cur->GetTerm()[0];

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

void AbstractSyntaxTree::Shift(std::shared_ptr<TermNode> &from, int64_t d_pos, int64_t cutoff) {
    if (from->GetType() == TermType::kVar) {
        auto cur = std::static_pointer_cast<Var>(from);
        if (static_cast<int64_t>(cur->GetDeBruijnIndex()) >= cutoff) {
            cur->SetDeBruijnIndex(cur->GetDeBruijnIndex() + d_pos);
        }
    } else if (from->GetType() == TermType::kAbs) {
        auto cur = std::static_pointer_cast<Abs>(from);
        Shift(cur->GetDown(), d_pos, cutoff + 1);

    } else if (from->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(from);
        Shift(cur->GetLeft(), d_pos, cutoff);
        Shift(cur->GetRight(), d_pos, cutoff);
    }
}

void AbstractSyntaxTree::Substitution(std::shared_ptr<TermNode> &term_to_reduce, std::shared_ptr<TermNode> &value, size_t j) {
    if (term_to_reduce->GetType() == TermType::kVar) {
        auto cur = std::static_pointer_cast<Var>(term_to_reduce);
        if (cur->GetDeBruijnIndex() == j) {
            auto parent = cur->GetParent().lock();
            auto new_value = CopySubTree(value);
            if (parent->GetType() == TermType::kAbs) {
                new_value->SetChildType(ChildType::kDown);
                auto parent_downcast = std::static_pointer_cast<Abs>(parent);
                new_value->SetParent(parent_downcast);
                parent_downcast->SetDown(new_value);
            } else if (parent->GetType() == TermType::kApp) {
                auto parent_downcast = std::static_pointer_cast<App>(parent);
                new_value->SetParent(parent_downcast);
                if (cur->GetChildType() == ChildType::kLeft) {
                    new_value->SetChildType(ChildType::kLeft);
                    parent_downcast->SetLeft(new_value);
                } else if (cur->GetChildType() == ChildType::kRight) {
                    new_value->SetChildType(ChildType::kRight);
                    parent_downcast->SetRight(new_value);
                }
            }
        }
    } else if (term_to_reduce->GetType() == TermType::kAbs) {
        auto cur = std::static_pointer_cast<Abs>(term_to_reduce);
        auto new_value = CopySubTree(value);
        Shift(new_value, 1, 0);
        Substitution(cur->GetDown(), new_value, j + 1);
    } else if (term_to_reduce->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(term_to_reduce);
        Substitution(cur->GetLeft(), value, j);
        Substitution(cur->GetRight(), value, j);
    }
}

std::shared_ptr<TermNode> AbstractSyntaxTree::CopySubTree(const std::shared_ptr<TermNode> &from) const {
    std::shared_ptr<TermNode> copied_sub_tree = std::shared_ptr<TermNode>(from);
    CopySubTreeRecursive(copied_sub_tree, from);
    return copied_sub_tree;
}

void AbstractSyntaxTree::CopySubTreeRecursive(std::shared_ptr<TermNode> &copy_node, const std::shared_ptr<TermNode> &from) const {
    if (from->GetType() == TermType::kVar) {
        auto parent = copy_node->GetParent();
        copy_node = std::make_shared<Var>(from->GetChildType(), from->GetTerm());
        auto cur_copy_node = std::static_pointer_cast<Var>(copy_node);
        auto cur_source_node = std::static_pointer_cast<Var>(from);
        cur_copy_node->SetParent(parent);
        cur_copy_node->SetIsFree(cur_source_node->IsFree());
        cur_copy_node->SetDeBruijnIndex(cur_source_node->GetDeBruijnIndex());
    } else if (from->GetType() == TermType::kAbs) {
        auto parent = copy_node->GetParent();
        copy_node = std::make_shared<Abs>(from->GetChildType(), from->GetTerm());
        auto cur_copy_node = std::static_pointer_cast<Abs>(copy_node);
        auto cur_source_node = std::static_pointer_cast<Abs>(from);

        cur_copy_node->SetParent(parent);
        cur_copy_node->SetDown(std::shared_ptr<TermNode>(cur_source_node->GetDown()));
        cur_copy_node->GetDown()->SetParent(cur_copy_node);
        CopySubTreeRecursive(cur_copy_node->GetDown(), cur_source_node->GetDown());
    } else if (from->GetType() == TermType::kApp) {
        auto parent = copy_node->GetParent();
        copy_node = std::make_shared<App>(from->GetChildType(), from->GetTerm());
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

std::pair<bool, std::shared_ptr<TermNode>> AbstractSyntaxTree::FindRedexInCallByValue(const std::shared_ptr<TermNode> &from) {
    if (from->GetType() == TermType::kAbs) {
        return {false, {}};
    }
    if (from->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(from);
        auto left_term = cur->GetLeft();
        auto right_term = cur->GetRight();
        if (left_term->GetType() == TermType::kAbs) {
            auto res_for_right = FindRedexInCallByValue(right_term);
            if (!res_for_right.first) {
                return {true, cur};
            } else {
                return res_for_right;
            }
        } else {
            return FindRedexInCallByValue(left_term);
        }
    }
    return {false, {}};
}

std::pair<bool, std::shared_ptr<TermNode>> AbstractSyntaxTree::FindRedexInCallByName(const std::shared_ptr<TermNode> &from) {
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
            return FindRedexInCallByName(left_term);
        }
    }
    return {false, {}};
}

std::pair<bool, std::shared_ptr<TermNode>> AbstractSyntaxTree::FindRedexInNormalStrategy(const std::shared_ptr<TermNode> &from) {
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

void AbstractSyntaxTree::MakeReductionStep(std::shared_ptr<TermNode> &from) {
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
        if (from->GetParent().lock()->GetType() == TermType::kAbs) {
            auto parent = std::static_pointer_cast<Abs>(from->GetParent().lock());
            parent->SetDown(new_reduced_node);
            new_reduced_node->SetParent(parent);
            new_reduced_node->SetChildType(ChildType::kDown);
        } else if (from->GetParent().lock()->GetType() == TermType::kApp) {
            auto parent = std::static_pointer_cast<App>(from->GetParent().lock());
            new_reduced_node->SetParent(parent);
            if (from->GetChildType() == ChildType::kLeft) {
                new_reduced_node->SetChildType(ChildType::kLeft);
                parent->SetLeft(new_reduced_node);
            } else if (from->GetChildType() == ChildType::kRight) {
                new_reduced_node->SetChildType(ChildType::kRight);
                parent->SetRight(new_reduced_node);
            }
        }
    }
}

std::pair<bool, std::pair<std::vector<std::string>,
                          std::vector<int64_t>>>
AbstractSyntaxTree::BetaReduction(StrategyType strategy_type, size_t steps_limit,
                                  size_t term_size_limit) {
    std::vector<std::string> reduction_steps;
    std::vector<int64_t> term_sizes;

//    reduction_steps.push_back(ExprToStringHaskell(this->root_));
    reduction_steps.push_back(ExprToStringDB(this->root_));
    term_sizes.push_back(CalculateTermSize(root_));

    size_t count_of_reduction_steps = 0;
    while ((count_of_reduction_steps < steps_limit) &&
           (term_sizes[count_of_reduction_steps] < static_cast<int64_t>(term_size_limit))) {

        std::pair<bool, std::shared_ptr<TermNode>> redex = {};
        if (strategy_type == StrategyType::kNormal) {
            redex = FindRedexInNormalStrategy(this->root_);
        } else if (strategy_type == StrategyType::kCallByName) {
            redex = FindRedexInCallByName(this->root_);
        } else if (strategy_type == StrategyType::kCallByValue) {
            redex = FindRedexInCallByValue(this->root_);
        }

        if (!redex.first) {
            break;
        }
        MakeReductionStep(redex.second);

//        auto reduced_term = ExprToStringHaskell(this->root_);
        auto reduced_term = ExprToStringDB(this->root_);
        if (std::find(reduction_steps.begin(), reduction_steps.end(), reduced_term) == reduction_steps.end()) {
            reduction_steps.push_back(reduced_term);
            term_sizes.push_back(CalculateTermSize(root_));
        } else {
            return {false, {reduction_steps, term_sizes}};
        }
        ++count_of_reduction_steps;
    }

    if ((count_of_reduction_steps >= steps_limit) ||
        (term_sizes[count_of_reduction_steps] >= static_cast<int64_t>(term_size_limit))) {
        return {false, {reduction_steps, term_sizes}};
    }
    return {true, {reduction_steps, term_sizes}};
}

int64_t AbstractSyntaxTree::CalculateTermSize(const std::shared_ptr<TermNode> &from) {
    if (from->GetType() == TermType::kAbs) {
        return 1 + CalculateTermSize(std::static_pointer_cast<Abs>(from)->GetDown());
    }
    if (from->GetType() == TermType::kApp) {
        auto left_term_size = CalculateTermSize(std::static_pointer_cast<App>(from)->GetLeft());
        auto right_term_size = CalculateTermSize(std::static_pointer_cast<App>(from)->GetRight());
        return 1 + left_term_size + right_term_size;
    }
    return 0;
}

std::string AbstractSyntaxTree::ExprToStringDB(const std::shared_ptr<TermNode> &from) {
    if (from->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(from);
        auto left_str = ExprToStringDB(cur->GetLeft());
        auto right_str = ExprToStringDB(cur->GetRight());
        return "(" + left_str + " " + right_str + ")";
    } else if (from->GetType() == TermType::kAbs) {
        auto cur = std::static_pointer_cast<Abs>(from);
        auto down_str = ExprToStringDB(cur->GetDown());
        return "(" + std::string("\\ ") + down_str + ")";
    } else if (from->GetType() == TermType::kVar) {
        auto cur = std::static_pointer_cast<Var>(from);
        return std::to_string(cur->GetDeBruijnIndex());
    }
    return {};
}

std::string AbstractSyntaxTree::ExprToStringHaskell(const std::shared_ptr<TermNode> &from) {
    if (from->GetType() == TermType::kApp) {
        auto cur = std::static_pointer_cast<App>(from);
        auto left_str = ExprToStringHaskell(cur->GetLeft());
        auto right_str = ExprToStringHaskell(cur->GetRight());
        return "(App " + left_str + " " + right_str + ")";
    } else if (from->GetType() == TermType::kAbs) {
        auto cur = std::static_pointer_cast<Abs>(from);
        auto down_str = ExprToStringHaskell(cur->GetDown());
        return "(Abs " + down_str + ")";
    } else if (from->GetType() == TermType::kVar) {
        auto cur = std::static_pointer_cast<Var>(from);
        return std::to_string(cur->GetDeBruijnIndex());
    }
    return {};
}

const std::shared_ptr<TermNode> &AbstractSyntaxTree::GetRoot() const {
    return root_;
}

std::shared_ptr<TermNode> &AbstractSyntaxTree::GetRoot() {
    return root_;
}
