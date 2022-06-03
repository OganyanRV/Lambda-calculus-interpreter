#ifndef OGANYAN_LAMBDA_CALC_TERMNODE_H
#define OGANYAN_LAMBDA_CALC_TERMNODE_H
#include <memory>

enum class TermType {
    kVar,
    kAbs,
    kApp
};

enum class ChildType {
    kLeft,
    kRight,
    kDown
};

class TermNode {
    friend class AbstractSyntaxTree;

protected:
    TermType type_ = TermType::kApp;
    ChildType child_type_ = ChildType::kDown;
    size_t begin_idx_, end_idx_;
    std::weak_ptr<TermNode> parent_;
    std::string term_;

public:
    TermNode() = default;
    TermNode(size_t begin_idx, size_t end_idx) : begin_idx_(begin_idx),
                                                 end_idx_(end_idx){};
    TermNode(TermType type, size_t begin_idx, size_t end_idx) : type_(type),
                                                                begin_idx_(begin_idx),
                                                                end_idx_(end_idx){};

    TermNode(ChildType child_type,
             size_t begin_idx,
             size_t end_idx) : child_type_(child_type),
                               begin_idx_(begin_idx),
                               end_idx_(end_idx){};

    TermNode(TermType type,
             ChildType child_type,
             size_t begin_idx,
             size_t end_idx) : type_(type),
                               child_type_(child_type),
                               begin_idx_(begin_idx),
                               end_idx_(end_idx){};

    TermNode(ChildType child_type,
             size_t begin_idx,
             size_t end_idx,
             const std::string &source_term) : child_type_(child_type),
                                               begin_idx_(begin_idx),
                                               end_idx_(end_idx),
                                               term_(source_term.substr(
                                                       begin_idx_,
                                                       end_idx - begin_idx_ + 1)){};

    TermNode(size_t begin_idx,
             size_t end_idx,
             const std::string &source_term) : begin_idx_(begin_idx),
                                               end_idx_(end_idx),
                                               term_(source_term.substr(
                                                       begin_idx_,
                                                       end_idx - begin_idx_ + 1)){};
    TermNode(TermType type,
             size_t begin_idx,
             size_t end_idx,
             const std::string &source_term) : type_(type),
                                               begin_idx_(begin_idx),
                                               end_idx_(end_idx),
                                               term_(source_term.substr(
                                                       begin_idx_,
                                                       end_idx - begin_idx_ + 1)) {}

    TermNode(TermType type, ChildType child_type,
             size_t begin_idx,
             size_t end_idx,
             const std::string &source_term) : type_(type),
                                               child_type_(child_type),
                                               begin_idx_(begin_idx),
                                               end_idx_(end_idx),
                                               term_(source_term.substr(
                                                       begin_idx_,
                                                       end_idx - begin_idx_ + 1)) {}

    std::string GetTerm() const {
        return term_;
    }
    TermType GetType() const {
        return type_;
    }
    size_t GetBeginIdx() const {
        return begin_idx_;
    }
    size_t GetEndIdx() const {
        return end_idx_;
    }
    ChildType GetChildType() const {
        return child_type_;
    }
    const std::weak_ptr<TermNode> &GetParent() const {
        return parent_;
    }
    void SetType(TermType type) {
        type_ = type;
    }
    void SetBeginIdx(size_t begin_idx) {
        begin_idx_ = begin_idx;
    }
    void SetEndIdx(size_t end_idx) {
        end_idx_ = end_idx;
    }
    void SetParent(const std::weak_ptr<TermNode> &parent) {
        parent_ = parent;
    }
    void SetTerm(const std::string &term) {
        term_ = term;
    }

    void SetChildType(ChildType child_type) {
        child_type_ = child_type;
    }


    virtual ~TermNode() = default;
};

class Var : public TermNode {
    size_t de_bruijn_index_;
    bool is_free_ = true;
    //    size_t alpha_conversion_count_;

public:
    Var() = default;
    Var(size_t begin_idx, size_t end_idx) : TermNode(TermType::kVar, begin_idx, end_idx){};
    Var(size_t begin_idx, size_t end_idx,
        const std::string &source_term) : TermNode(TermType::kVar, begin_idx, end_idx, source_term){};
    Var(ChildType child_type, size_t begin_idx, size_t end_idx) : TermNode(TermType::kVar, child_type, begin_idx, end_idx){};
    Var(ChildType child_type, size_t begin_idx,
        size_t end_idx, const std::string &source_term) : TermNode(TermType::kVar, child_type,
                                                                   begin_idx, end_idx, source_term){};

    size_t GetDeBruijnIndex() const {
        return de_bruijn_index_;
    }
    bool IsFree() const {
        return is_free_;
    }
    void SetDeBruijnIndex(size_t de_bruijn_index) {
        de_bruijn_index_ = de_bruijn_index;
    }
    void SetIsFree(bool is_free) {
        is_free_ = is_free;
    }
};

class Abs : public TermNode {
private:
    std::shared_ptr<TermNode> down_;

public:
    Abs() = default;
    Abs(size_t begin_idx, size_t end_idx) : TermNode(TermType::kAbs, begin_idx, end_idx){};
    Abs(size_t begin_idx, size_t end_idx,
        const std::string &source_term) : TermNode(TermType::kAbs, begin_idx, end_idx, source_term){};
    Abs(ChildType child_type, size_t begin_idx, size_t end_idx) : TermNode(TermType::kAbs, child_type, begin_idx, end_idx){};
    Abs(ChildType child_type, size_t begin_idx,
        size_t end_idx, const std::string &source_term) : TermNode(TermType::kAbs, child_type,
                                                                   begin_idx, end_idx, source_term){};

    const std::shared_ptr<TermNode> &GetDown() const {
        return down_;
    }

    std::shared_ptr<TermNode> &GetDown() {
        return down_;
    }


    void SetDown(const std::shared_ptr<TermNode> &down) {
        down_ = down;
    }
};

class App : public TermNode {
private:
    std::shared_ptr<TermNode> left_;
    std::shared_ptr<TermNode> right_;

public:
    App() = default;
    App(size_t begin_idx, size_t end_idx) : TermNode(TermType::kApp, begin_idx, end_idx){};
    App(size_t begin_idx, size_t end_idx,
        const std::string &source_term) : TermNode(TermType::kApp, begin_idx, end_idx, source_term){};
    App(ChildType child_type, size_t begin_idx, size_t end_idx) : TermNode(TermType::kApp, child_type, begin_idx, end_idx){};
    App(ChildType child_type, size_t begin_idx,
        size_t end_idx, const std::string &source_term) : TermNode(TermType::kApp, child_type,
                                                                   begin_idx, end_idx, source_term){};

    const std::shared_ptr<TermNode> &GetLeft() const {
        return left_;
    }

    const std::shared_ptr<TermNode> &GetRight() const {
        return right_;
    }

    std::shared_ptr<TermNode> &GetLeft() {
        return left_;
    }

    std::shared_ptr<TermNode> &GetRight() {
        return right_;
    }


    void SetLeft(const std::shared_ptr<TermNode> &left) {
        left_ = left;
    }

    void SetRight(const std::shared_ptr<TermNode> &right) {
        right_ = right;
    }
};


#endif//OGANYAN_LAMBDA_CALC_TERMNODE_H
