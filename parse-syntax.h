#ifndef OGANYAN_LAMBDA_CALC_PARSE_SYNTAX_H
#define OGANYAN_LAMBDA_CALC_PARSE_SYNTAX_H

std::string PutBracketsAroundLambdaAbstractions(const std::string &term) {
    std::string corrected_term{term};
    while (true) {
        std::string new_term;
        bool changed = false;
        for (size_t pos_for_opening_bracket = 0;
             pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
            if (
                    (corrected_term[pos_for_opening_bracket] == '\\' && (!pos_for_opening_bracket)) ||
                    (corrected_term[pos_for_opening_bracket] == '\\' &&
                     (corrected_term[pos_for_opening_bracket - 1] != '('))) {
                int brackets_count = 0;
                int pos_for_ending_bracket = corrected_term.size();
                for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                    if (corrected_term[j] == '(') {
                        ++brackets_count;
                    } else if (corrected_term[j] == ')') {
                        --brackets_count;
                    }
                    if (brackets_count < 0) {
                        pos_for_ending_bracket = j;
                        break;
                    }
                }
                new_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                           corrected_term.substr(pos_for_opening_bracket,
                                                 pos_for_ending_bracket - pos_for_opening_bracket) +
                           ')' +
                           corrected_term.substr(pos_for_ending_bracket,
                                                 corrected_term.size() - pos_for_ending_bracket);
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
        corrected_term = new_term;
    }
    return corrected_term;
}

std::string PutBracketsAroundSubTermsInLambdaAbstractions(const std::string &term) {
    std::string corrected_term{term};
    while (true) {
        std::string new_term;
        bool changed = false;

        for (size_t pos_for_opening_bracket = 0;
             pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
            if (corrected_term[pos_for_opening_bracket] == '\\' &&
                corrected_term[pos_for_opening_bracket + 3] != '(') {
                int brackets_count = 0;
                int pos_for_ending_bracket = corrected_term.size();
                for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                    if (corrected_term[j] == '(') {
                        ++brackets_count;
                    } else if (corrected_term[j] == ')') {
                        --brackets_count;
                    }
                    if (brackets_count < 0) {
                        pos_for_ending_bracket = j;
                        break;
                    }
                }
                new_term = corrected_term.substr(0, pos_for_opening_bracket + 3) + '(' +
                           corrected_term.substr(pos_for_opening_bracket + 3,
                                                 pos_for_ending_bracket - pos_for_opening_bracket - 3) +
                           ')' +
                           corrected_term.substr(pos_for_ending_bracket,
                                                 corrected_term.size() - pos_for_ending_bracket);
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
        corrected_term = new_term;
    }
    return corrected_term;
}

std::string PutBracketsInApplications(const std::string &term) {
    std::string corrected_term{term};


    size_t count_of_inserted_brackets_pairs = 0;
    for (size_t pos_for_opening_bracket = 0;
         pos_for_opening_bracket < corrected_term.size(); ++pos_for_opening_bracket) {
        count_of_inserted_brackets_pairs = 0;
        if (corrected_term[pos_for_opening_bracket] == '(') {
            int brackets_count = 0;
            int pos_for_ending_bracket = corrected_term.size();
            for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size(); ++j) {
                if (corrected_term[j] == '(') {
                    ++brackets_count;
                } else if (corrected_term[j] == ')') {
                    --brackets_count;
                }
                if (brackets_count < 0) {
                    pos_for_ending_bracket = j;
                    break;
                }
            }

            for (size_t j = pos_for_ending_bracket + 1; j < corrected_term.size(); ++j) {
                if (corrected_term[j] == ' ') {
                    continue;
                }

                if (corrected_term[j] == ')') {
                    break;
                }

                // (\x x) a => ((\x x) a)
                if (std::isalpha(corrected_term[j])) {
                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket + 1) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket + 1,
                                                           j - pos_for_opening_bracket) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }

                // (\x x) (\y y) a => ((\x x) (\y y)) a
                if (corrected_term[j] == '(') {
                    brackets_count = 0;
                    ++j;
                    while (j < corrected_term.size()) {
                        if (corrected_term[j] == '(') {
                            ++brackets_count;
                        } else if (corrected_term[j] == ')') {
                            --brackets_count;
                        }
                        if (brackets_count < 0) {
                            pos_for_ending_bracket = j;
                            break;
                        }
                        ++j;
                    }

                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket + 1) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket + 1,
                                                           pos_for_ending_bracket - pos_for_opening_bracket) +
                                     ')' +
                                     corrected_term.substr(pos_for_ending_bracket + 1,
                                                           corrected_term.size() - pos_for_ending_bracket - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }
            }

            pos_for_opening_bracket += count_of_inserted_brackets_pairs;
        }
        //        else if (std::isalpha(corrected_term[pos_for_opening_bracket])) {
        //        else if ((!pos_for_opening_bracket && std::isalpha(corrected_term[pos_for_opening_bracket]))
        //                 || (std::isalpha(corrected_term[pos_for_opening_bracket]) && pos_for_opening_bracket
        //                 && corrected_term[pos_for_opening_bracket - 1] != '\\')) {

        else if (std::isalpha(corrected_term[pos_for_opening_bracket]) &&
                 (!pos_for_opening_bracket ||
                  (pos_for_opening_bracket && corrected_term[pos_for_opening_bracket - 1] != '\\'))) {
            bool done = false;
            for (size_t j = pos_for_opening_bracket + 1; j < corrected_term.size() && !done; ++j) {
                if (corrected_term[j] == ')') {
                    done = true;
                }

                if (std::isalpha(corrected_term[j])) {
                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket,
                                                           j - pos_for_opening_bracket + 1) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }

                if (corrected_term[j] == '(') {
                    int brackets_count = 0;
                    int pos_for_ending_bracket = corrected_term.size();
                    ++j;
                    while (j < corrected_term.size()) {
                        if (corrected_term[j] == '(') {
                            ++brackets_count;
                        } else if (corrected_term[j] == ')') {
                            --brackets_count;
                        }
                        if (brackets_count < 0) {
                            pos_for_ending_bracket = j;
                            break;
                        }
                        ++j;
                    }

                    corrected_term = corrected_term.substr(0, pos_for_opening_bracket) + '(' +
                                     corrected_term.substr(pos_for_opening_bracket,
                                                           j - pos_for_opening_bracket + 1) +
                                     ')' +
                                     corrected_term.substr(j + 1,
                                                           corrected_term.size() - j - 1);
                    j += 2;
                    ++count_of_inserted_brackets_pairs;
                    continue;
                }
            }
            pos_for_opening_bracket += count_of_inserted_brackets_pairs;
        }
    }
    return corrected_term;
}

std::string RemoveSpaces(const std::string &term) {
    std::string new_term;
    for (auto &elem : term) {
        if (elem != ' ') {
            new_term += elem;
        }
    }
    return new_term;
}

//  Makes appropriate form of expression
std::string MakeCorrectForm(const std::string &term) {
    std::string corrected_term{term};
//    std::cout << "Source expr: " << corrected_term << "\n";
    corrected_term = PutBracketsAroundLambdaAbstractions(corrected_term);
//    std::cout << "Expr after putting brackets arround abstractions: " << corrected_term << "\n";
    corrected_term = PutBracketsAroundSubTermsInLambdaAbstractions(corrected_term);
//    std::cout << "Expr after putting brackets arround subterm in abstractions: " << corrected_term << "\n";
    corrected_term = PutBracketsInApplications(corrected_term);
//    std::cout << "Expr after putting brackets in applications: " << corrected_term << "\n";
    // Maybe make function for removing extra brackets
    return corrected_term;
}

#endif//OGANYAN_LAMBDA_CALC_PARSE_SYNTAX_H
