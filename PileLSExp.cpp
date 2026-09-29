#include <iostream>
#include <string>
#include "PileGenericTabDy.hpp"

bool verifierParenthese(const std::string& expression) {
    Pile<char> cPile;

    for (char caractere : expression) {
        if (caractere == '(') {
            cPile.empiler(caractere);
        }
        else if (caractere == ')') {
            if (!cPile.depiler()) {
                return false;
            }
        }
    }
    return cPile.estVide();
}

int main() {
    const std::string expression = "(x+y)";
    const std::string expression2 = "(x+y";
    std::cout << expression << (verifierParenthese(expression) ? " Valide" : " Invalide") << '\n';
    std::cout << expression2 << (verifierParenthese(expression2) ? " Valide" : " Invalide") << '\n';
}