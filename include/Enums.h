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
    kHaskell
};

enum class StrategyType {
    kNormal,
    kCallByName,
    kCallByValue,
    kAll
};

enum class NormalFormType {
    kExisting,
    kCycling,
    kIncreasing
};

#endif//OGANYAN_LAMBDA_CALC_ENUMS_H
