### List of ideas TODO
1. Support input terms with de bruijn notation. Interpreter should be able to parse syntax and build abstract tree (Its similar to normal term style, but I dont have time to make it).
In that case edit here:
   1. Rules
    ```c++
    std::cout << "-+-+-+-+-+-+-+-+-+-+  Rules  -+-+-+-+-+-+-+-+-+-+-+-+-+"
                      << "\n";
            std::cout << "1. Capital letters are only used for Library Functions such as True, False, Not ..."
                      << "\n";
            std::cout << "2. Be accurate with brackets. Do not mess up with brackets"
                      << "\n";
            std::cout
                    << "3. There are some checks for correct syntax, but they dont cover all possible mistake."
                       " Do your best to not make any mistakes"
                    << "\n";
            std::cout << "4. The Interpreter supports 3 types of input: classic term, "
                         "term in de bruijn notation, haskell-style term"
                      << "\n\t4.1 Classic term:"
                      << "\n\t\t4.1.1 Variables can only be named with single cursive letters"
                      << "\n\t\t4.1.2 Applications splits with spaces."
                      << "\n\t\t4.1.3 Abstractions looks like this: \\x z x y, where x is formal parameter "
                         "and 'z x y' is abstraction's body"
                      << "\n\t\t4.1.4 Example: (\\x x) (\\y y)"
    
                      << "\n\t4.2 Term in de Bruijn notation:"
                      << "\n\t\t4.2.1 Variables can only be named with numbers"
                      << "\n\t\t4.2.2 Applications splits with spaces."
                      << "\n\t\t4.2.3 Abstractions looks like this: \\ 0 1 2, where '0 1 2' is abstraction's body;"
                         " 0 means bounded variable, 1 and 2 are some free vars"
                      << "\n\t\t4.2.4 Example: (\\ 0) (\\ 0)"
    
                      << "\n\t4.3 Haskell-style term:"
                      << "\n\t\t4.3.1 Variables can only be named with numbers"
                      << "\n\t\t4.3.2 Applications splits with spaces."
                      << "\n\t\t4.3.3 Applications looks like this: App(term term)"
                      << "\n\t\t4.3.4 Abstractions looks like this: Abs(term)"
                      << "\n\t\t4.3.5 Example: (App((Abs(0) (Abs(0)))"
                      << "\n";
    
            return;
    ```
   2. Beta-reduction interpreter
    ```c++
     std::cout << "Beta-reduction's interpreter. Options:\n"
                      << "1. There are 3 possible types of input:\n"
                      << "\t1.1 Type '1' for classic term style. Example: (\\x x) (\\y y)\n"
                      << "\t1.2 Type '2' for term in de Bruijn notation style. Example: (\\ 0) (\\ 0)\n"
                      << "\t1.3 Type '3' for Haskell-style term. Example: (App((Abs(0) (Abs(0)))\n"
                      << "2. Type 'exit' to exit beta-reduction's interpreter\n";
    ```
   3. Enum
   ```c++
    enum class InputType {
    kNormal,
    kHaskell,
    kDeBruijn // TODO
    };
    ```
   4. Build tree fucntion
   ```c++
    void BuildTreeDeBruijn(std::shared_ptr<TermNode> &from, size_t begin_idx, size_t end_idx);
    ```
   5. And other places.

2. Support printing normal-style terms after reduction (at this moment i store everything in de bruijn notation and have no option to see reduced "named" term. For example (\x \y x y) (\y y) reduces to (\ (\ 0) 0) and then reduces to (\ 0). In my program it shows like that (just de Bruijn notation) or Haskell-style De Bruijn notation (Abs 0), but we also want to print term with letters (\x \y x y) (\y y) => (\y (\y' y') y) => (\y y)
