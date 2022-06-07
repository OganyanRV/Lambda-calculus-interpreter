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
    std::weak_ptr<TermNode> parent_ = (std::weak_ptr<TermNode>());
    std::string term_;

public:
    TermNode() = default;
    virtual ~TermNode() = default;
    TermNode(ChildType child_type) : child_type_(child_type){}
    TermNode(TermType type) : type_(type){}
    TermNode(const std::string &term) : term_(term){}
    TermNode(TermType type,
             ChildType child_type) : type_(type),
                                     child_type_(child_type){}
    TermNode(ChildType child_type,
             const std::string &term) : child_type_(child_type),
                                        term_(term){}
    TermNode(TermType type,
             const std::string &term) : type_(type),
                                        term_(term) {}
    TermNode(TermType type, ChildType child_type,
             const std::string &term) : type_(type),
                                        child_type_(child_type),
                                        term_(term) {}

    std::string GetTerm() const {
        return term_;
    }
    TermType GetType() const {
        return type_;
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
    void SetParent(const std::weak_ptr<TermNode> &parent) {
        parent_ = parent;
    }
    void SetTerm(const std::string &term) {
        term_ = term;
    }
    void SetChildType(ChildType child_type) {
        child_type_ = child_type;
    }
};

class Var : public TermNode {
private:
    size_t de_bruijn_index_;
    bool is_free_ = true; // ?

public:
    Var() : TermNode(TermType::kVar){}
    Var(const std::string &term) : TermNode(TermType::kVar, term){}
    Var(ChildType child_type) : TermNode(TermType::kVar, child_type){}
    Var(ChildType child_type, const std::string &term) : TermNode(TermType::kVar, child_type,
                                                                  term){}

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
    Abs() : TermNode(TermType::kAbs){}
    Abs(const std::string &term) : TermNode(TermType::kAbs, term){}
    Abs(ChildType child_type) : TermNode(TermType::kAbs, child_type){}
    Abs(ChildType child_type, const std::string &term) : TermNode(TermType::kAbs, child_type, term){}


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
    App() : TermNode(TermType::kApp){}
    App(ChildType child_type) : TermNode(TermType::kApp, child_type){}
    App(const std::string &term) : TermNode(TermType::kApp, term){}
    App(ChildType child_type, const std::string &term) : TermNode(TermType::kApp, child_type,
                                                                  term){}

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
