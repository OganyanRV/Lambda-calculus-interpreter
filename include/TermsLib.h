#ifndef OGANYAN_LAMBDA_CALC_TERMSLIB_H
#define OGANYAN_LAMBDA_CALC_TERMSLIB_H
#include <map>
#include <string>

class TermsLib {
private:
    std::map<std::string, std::string> combinators_lib_;

public:
    TermsLib() {
        // Logical functions
        combinators_lib_["True"] = "(\\t (\\f t))";
        combinators_lib_["False"] = "(\\t (\\f f))";
        combinators_lib_["If"] = "(\\b (\\x (\\y b x y)))";
        combinators_lib_["And"] = "(\\b (\\c b c False))";
        combinators_lib_["Or"] = "(\\b (\\c b True c))";
        combinators_lib_["Not"] = "(\\b b False True)";
        combinators_lib_["IsZero"] = "(\\n n (\\c False) True)";

        // Nums
        combinators_lib_["Zero"] = "(\\s (\\z z))";
        combinators_lib_["One"] = "(\\s (\\z s z))";
        combinators_lib_["Two"] = "(\\s (\\z s (s z)))";

        // Arithmetic functions
        combinators_lib_["Scc"] = "(\\n (\\s (\\z s (n s z))))";
        combinators_lib_["Plus"] = "(\\n (\\m (\\s (\\z (n s (m s z))))))";
        combinators_lib_["Mult"] = "(\\n (\\m (\\s n (m s))))";
        combinators_lib_["Pow"] = "(\\n (\\m (\\s (\\ z (n s z)))))";

        //  Combinators
        combinators_lib_["I"] = "(\\x x)";
        combinators_lib_["S"] = "(\\f (\\g (\\x f x (g x))))";
        combinators_lib_["K"] = "(\\x (\\y x))";
        combinators_lib_["Omega"] = "((\\x x x)(\\x x x))";
        combinators_lib_["B"] = "(\\f (\\g (\\x f (g x))))";
        combinators_lib_["C"] = "(\\f (\\x (\\y f y x)))";
        combinators_lib_["V"] = "(\\x (\\y x y y))";


        //  Pairs
        combinators_lib_["Pair"] = "(\\f (\\s (\\b b f s)))";
        combinators_lib_["First"] = "(\\p p True)";
        combinators_lib_["Second"] = "(\\p p False)";
    }

    bool Exist(const std::string &term_name) const {
        return (combinators_lib_.find(term_name) != combinators_lib_.end());
    }

    std::string operator[](const std::string &term_name) {
        if (Exist(term_name)) {
            return combinators_lib_[term_name];
        } else {
            return "";
        }
    }

    std::string ChangeLibFuncsToTerms(std::string source_term) {
        for (size_t idx = 0; idx < source_term.size(); ++idx) {
            size_t begin_idx;
            if (source_term[idx] >= 'A' && source_term[idx] <= 'Z') {
                begin_idx = idx;
                while (idx < source_term.size() && std::isalpha(source_term[idx])) {
                    ++idx;
                }
                std::string lib_term = source_term.substr(begin_idx, idx - begin_idx);
                std::string term_to_replace = combinators_lib_[lib_term];
                source_term = source_term.substr(0, begin_idx) + term_to_replace + source_term.substr(idx);
                idx = 0;
            }
        }
        return source_term;
    }
};
#endif//OGANYAN_LAMBDA_CALC_TERMSLIB_H
