#include "PileGenericTabDy.hpp"

#include <iostream>
#include <string>

bool verifierParentheses(const std::string& expression) {
    Pile<char> pile;

    for (char caractere : expression) {
        if (caractere == '(') {
            pile.empiler(caractere);
        } else if (caractere == ')' && !pile.depiler()) {
            return false;
        }
    }

    return pile.estVide();
}

int main() {
    const std::string expressions[] = {"(x+y)", "(x+y", "x+y)"};

    for (const std::string& expression : expressions) {
        std::cout << expression << " : "
                  << (verifierParentheses(expression) ? "valide" : "invalide")
                  << '\n';
    }
}