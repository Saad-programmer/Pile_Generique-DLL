#ifndef PILEGENERIC_H
#define PILEGENERIC_H

#include <cstddef>
#include <string>

#if defined(_WIN32)
#if defined(PILE_BUILDING_LIBRARY)
#define PILE_API __declspec(dllexport)
#else
#define PILE_API __declspec(dllimport)
#endif
#else
#define PILE_API
#endif

template <typename T>
class PILE_API Pile {
public:
    Pile();
    ~Pile();

    Pile(const Pile&) = delete;
    Pile& operator=(const Pile&) = delete;

    bool estVide() const;
    std::size_t taille() const;
    void empiler(const T& valeur);
    bool depiler();
    T consulterSommet() const;

private:
    void redimensionner(std::size_t nouvelleCapacite);

    T* donnees;
    std::size_t capacite;
    std::size_t sommet;
};

extern template class PILE_API Pile<char>;
extern template class PILE_API Pile<int>;
extern template class PILE_API Pile<double>;
extern template class PILE_API Pile<std::string>;

#endif