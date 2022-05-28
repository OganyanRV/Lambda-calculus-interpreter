#ifndef OGANYAN_LAMBDA_CALC_TERMNODE_H
#define OGANYAN_LAMBDA_CALC_TERMNODE_H

class TermNode {
public:
    virtual ~TermNode() = default;
};

class Var : public TermNode {
private:
public:

};

class Abs: public TermNode {
private:
public:

};

class App: public TermNode {
private:
public:

};


#endif //OGANYAN_LAMBDA_CALC_TERMNODE_H
