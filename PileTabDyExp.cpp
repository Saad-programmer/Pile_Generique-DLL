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
    const std::string mot = "x+y+x)";
    std::cout << mot << (verifierParenthese(mot) ? " Est valide" : " Invalide") << '\n';
}