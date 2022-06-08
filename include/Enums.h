#ifndef OGANYAN_LAMBDA_CALC_ENUMS_H
#define OGANYAN_LAMBDA_CALC_ENUMS_H

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

enum class InputType {
    kNormal,
    kDeBruijn,
    kHaskell
};

#endif//OGANYAN_LAMBDA_CALC_ENUMS_H
