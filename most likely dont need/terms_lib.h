#ifndef OGANYAN_LAMBDA_CALC_TERMS_LIB_H
#define OGANYAN_LAMBDA_CALC_TERMS_LIB_H

#include <iostream>
#include <string>
#include <map>

class LibFuncs {
private:
    std::map<std::string, std::string> functions_;
public:
    LibFuncs() {

        // Logical functions

        functions_["True"] = "(\\t (\\f t))";
        functions_["False"] = "(\\t (\\f f))";
        functions_["If"] = "(\\b (\\x (\\y b x y)))";
        functions_["And"] = "(\\b (\\c b c False))";
        functions_["Or"] = "(\\b (\\c b True c))";
        functions_["Not"] = "(\\b b False True)";
        functions_["IsZero"] = "(\\n n (\\c False) True)";

        // Nums

        functions_["Zero"] = "(\\s (\\z z))";
        functions_["One"] = "(\\s (\\z s z))";
        functions_["Two"] = "(\\s (\\z s (s z)))";

        // Arithmetic functions

        functions_["Scc"] = "(\\n (\\s (\\z s (n s z))))";
        functions_["Plus"] = "(\\n (\\m (\\s (\\z (n s (m s z))))))";
        functions_["Mult"] = "(\\n (\\m (\\s n (m s))))";
        functions_["Pow"] = "(\\n (\\m (\\s (\\ z (n s z)))))";

        //  Combinators

        functions_["I"] = "(\\x x)";
        functions_["S"] = "(\\f (\\g (\\x f x (g x))))";
        functions_["K"] = "(\\x (\\y x))";
        functions_["Omega"] = "((\\x x x)(\\x x x))";
        functions_["B"] = "(\\f (\\g (\\x f (g x))))";
        functions_["C"] = "(\\f (\\x (\\y f y x)))";
        functions_["V"] = "(\\x (\\y x y y))";


        //  Pairs
        functions_["Pair"] = "(\\f (\\s (\\b b f s)))";
        functions_["First"] = "(\\p p True)";
        functions_["Second"] = "(\\p p False)";
    }

    bool exist(const std::string &str) const {
        return (functions_.find(str) != functions_.end());
    }

    //  Not string& to avoid changing data
    std::string operator[](const std::string &index) {
        if (exist(index)) {
            return functions_[index];
        }
        else {
            return "";
        }
    }

    std::
};

#endif //OGANYAN_LAMBDA_CALC_TERMS_LIB_H
