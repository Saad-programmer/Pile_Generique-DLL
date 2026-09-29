#include "PileGenericTabDy.hpp"

#include <cassert>

int main() {
    Pile<int> pile;
    assert(pile.estVide());
    assert(!pile.depiler());

    for (int valeur = 0; valeur < 10; ++valeur) {
        pile.empiler(valeur);
    }

    assert(pile.taille() == 10);
    for (int valeur = 9; valeur >= 0; --valeur) {
        assert(pile.consulterSommet() == valeur);
        assert(pile.depiler());
    }
    assert(pile.estVide());
}