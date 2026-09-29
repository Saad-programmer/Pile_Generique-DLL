#include "PileGenericTabDy.hpp"

#include <stdexcept>

template <typename T>
Pile<T>::Pile() : donnees(new T[2]), capacite(2), sommet(0) {}

template <typename T>
Pile<T>::~Pile() {
    delete[] donnees;
}

template <typename T>
bool Pile<T>::estVide() const {
    return sommet == 0;
}

template <typename T>
std::size_t Pile<T>::taille() const {
    return sommet;
}

template <typename T>
void Pile<T>::redimensionner(std::size_t nouvelleCapacite) {
    T* nouvellesDonnees = new T[nouvelleCapacite];
    for (std::size_t index = 0; index < sommet; ++index) {
        nouvellesDonnees[index] = donnees[index];
    }
    delete[] donnees;
    donnees = nouvellesDonnees;
    capacite = nouvelleCapacite;
}

template <typename T>
void Pile<T>::empiler(const T& valeur) {
    if (sommet == capacite) {
        redimensionner(capacite * 2);
    }
    donnees[sommet++] = valeur;
}

template <typename T>
bool Pile<T>::depiler() {
    if (estVide()) {
        return false;
    }
    --sommet;
    return true;
}

template <typename T>
T Pile<T>::consulterSommet() const {
    if (estVide()) {
        throw std::underflow_error("Impossible de consulter une pile vide");
    }
    return donnees[sommet - 1];
}

template class PILE_API Pile<char>;
template class PILE_API Pile<int>;
template class PILE_API Pile<double>;
template class PILE_API Pile<std::string>;