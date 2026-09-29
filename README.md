# Construire et lancer le projet sans outil de build

Ce guide utilise Linux et `g++`. Il n'utilise ni CMake ni Makefile. Le programme est construit manuellement, et la pile est fournie par une bibliotheque partagee `libpile.so`.

## Fichiers utiles

- `PileGenericTabDy.hpp` : declarations visibles par le programme principal. Ce fichier indique les fonctions disponibles, mais ne contient pas leur implementation.
- `PileGenericTabDy.cpp` : implementation de `Pile` et instanciations des types exportes (`char`, `int`, `double`, `std::string`). C'est ce fichier qui devient la bibliotheque partagee.
- `main.cpp` : programme principal. Il utilise `Pile<char>` sans compiler l'implementation de la pile dans son executable.
- `build/` : fichiers intermediaires et resultats generes pendant les commandes ci-dessous.

Les deux exemples plus anciens (`PileLSExp.cpp` et `PileTabDyExp.cpp`) ne sont pas necessaires pour suivre cette procedure.

## 1. Verifier le compilateur

Commande :

```sh
g++ --version
```

Sortie de la machine ou ce guide a ete verifie :

```text
g++ (GCC) 15.2.0
```

`g++` est le pilote GNU C++ : il appelle les outils necessaires (preprocesseur, compilateur, assembleur et editeur de liens) selon les options donnees. Ici, on va d'abord appeler explicitement les etapes pour voir leurs fichiers intermediaires.

Creer le dossier de sortie :

```sh
mkdir -p build
```
Ouvrir d'abord un terminal dans le dossier qui contient `PileGenericTabDy.cpp` et `main.cpp`. Les commandes ci-dessous supposent que le dossier courant est ce dossier. Pour verifier :

```sh
pwd
```

La sortie est le chemin du dossier du projet. Les commandes sont specifiques a Linux/ELF; Windows utilise d'autres options et produit un fichier `.dll`.

## 2. Pretraiter l'implementation de la pile

```sh
g++ -std=c++17 -E PileGenericTabDy.cpp -o build/PileGenericTabDy.ii
```

`-E` arrete le travail apres le preprocesseur. Il lit `#include`, insere le contenu des en-tetes, developpe les macros et traite les directives conditionnelles. Il ne produit ni code machine ni programme executable. `-o` indique le nom du fichier de sortie.

Le fichier produit est `build/PileGenericTabDy.ii`. Il est beaucoup plus long que le source car il contient aussi le texte des en-tetes standards. Le compter et en afficher le debut :

```sh
wc -l build/PileGenericTabDy.ii
head -n 8 build/PileGenericTabDy.ii
```

`wc -l` affiche un nombre de lignes qui depend de la version des en-tetes installes. Les premieres lignes sont generalement des marqueurs de fichier ajoutes par le preprocesseur. Cette sortie est du texte C++ developpe, pas un objet executable.

## 3. Compiler le source en assembleur

```sh
g++ -std=c++17 -fPIC -S PileGenericTabDy.cpp -o build/PileGenericTabDy.s
```

Cette etape lit le C++ pretraite, verifie la syntaxe et les types, instancie le code pour les types explicitement demandes dans le source, puis produit des instructions assembleur dans `build/PileGenericTabDy.s`.

- `-S` demande de s'arreter apres la generation assembleur.
- `-fPIC` demande du code independant de sa position en memoire. C'est le format adapte au code place dans une bibliotheque partagee, qui peut etre chargee a une adresse choisie au lancement.

On peut voir le debut du texte assembleur avec :

```sh
head -n 20 build/PileGenericTabDy.s
```

Le texte varie selon le processeur, GCC et ses options. Ce n'est pas encore le fichier `.so`.

## 4. Assembler en fichier objet

```sh
as build/PileGenericTabDy.s -o build/PileGenericTabDy.o
```

L'assembleur `as` transforme les instructions du fichier `.s` en octets machine et cree `build/PileGenericTabDy.o`. Un fichier objet contient du code machine, une table de symboles et des references qui ne sont pas encore resolues, par exemple les appels aux fonctions de la bibliotheque C++ standard. Ce n'est pas un programme que l'on lance directement.

Verifier son type :

```sh
file build/PileGenericTabDy.o
```

Le resultat indique un fichier `ELF relocatable`, c'est-a-dire un objet Linux relogeable.

## 5. Fabriquer la bibliotheque partagee

```sh
g++ -shared -o build/libpile.so build/PileGenericTabDy.o
```

L'editeur de liens regroupe l'objet et construit `build/libpile.so`. `-shared` demande une bibliotheque partagee au lieu d'un executable. La bibliotheque expose les fonctions de `Pile` pour les types explicitement instancies. Elle ne contient pas `main` et ne peut pas etre executee seule.

Verifier le fichier et les symboles exportes :

```sh
```

`file` doit annoncer un objet partage ELF. `nm` liste les symboles; `-D` selectionne ceux visibles dynamiquement et `-C` rend leurs noms C++ lisibles. La ligne trouvee confirme que l'implementation de `empiler(char)` est disponible dans la bibliotheque.

## 6. Pretraiter, compiler et assembler `main.cpp`

Pretraitement :

```sh
g++ -std=c++17 -E main.cpp -o build/main.ii
```

Le preprocesseur insere notamment `PileGenericTabDy.hpp`. Ce fichier apporte les declarations de la classe. Il n'insere pas l'implementation de `Pile`, qui est compilee dans `libpile.so`.

Generation assembleur :

```sh
g++ -std=c++17 -S main.cpp -o build/main.s
```

Ici, `main.cpp` utilise la declaration `Pile<char>`. Les lignes `extern template` du header indiquent que cette instanciation est fournie ailleurs; le compilateur ne recopie donc pas l'implementation dans `main`.

Assemblage :

```sh
as build/main.s -o build/main.o
```

`build/main.o` contient le code machine de `main` et des references non resolues vers les fonctions de `Pile<char>`. C'est normal : elles se trouvent dans `libpile.so` et l'etape suivante enregistre cette dependance.

## 7. Edition de liens de l'executable

```sh
g++ -o build/main build/main.o -Lbuild -lpile -Wl,-rpath,'$ORIGIN'
```

Voici le role de chaque partie :

- `g++` lance l'editeur de liens avec les bibliotheques C++ necessaires.
- `-o build/main` nomme le programme executable cree.
- `build/main.o` apporte la fonction `main`.
- `-Lbuild` ajoute le dossier `build/` aux emplacements ou chercher les bibliotheques pendant cette edition de liens.
- `-lpile` demande la bibliotheque nommee `libpile.so` (`-l` ajoute le prefixe `lib` et cherche le nom correspondant).
- `-Wl,-rpath,'$ORIGIN'` transmet `-rpath,$ORIGIN` a l'editeur de liens. `$ORIGIN` signifie le dossier ou se trouve l'executable. Cela permet au chargeur de retrouver `libpile.so` dans le meme dossier `build/`, meme si on lance le programme depuis le dossier du projet.

A ce stade, l'editeur de liens verifie que les symboles utilises par `main.o` existent dans la bibliotheque et inscrit le nom `libpile.so` comme dependance de `build/main`. Il ne copie pas le contenu complet de la pile dans l'executable.

Verifier la dependance enregistree :

```sh
readelf -d build/main | grep NEEDED
```

Une ligne importante doit etre presente :

```text
Shared library: [libpile.so]
```

Verifier aussi le chemin de recherche enregistre :

```sh
readelf -d build/main | grep -E 'RPATH|RUNPATH'
```

La sortie doit contenir `$ORIGIN`.

## 8. Lancement et chargement a l'execution

```sh
./build/main
```

Sortie :

```text

Pour observer le chemin trouve par le systeme :

```sh
ldd build/main
```

La liste doit contenir une ligne semblable a :

```text
libpile.so => /.../ProblematiqueDLL/build/libpile.so
```

Le chemin exact depend de l'emplacement du projet.

## Les trois notions a ne pas confondre

1. **Compilation** : transforme chaque fichier C++ en fichier objet (`.o`), apres les etapes de pretraitement et de generation assembleur.
2. **Edition de liens** : combine `main.o` et les references a `libpile.so` pour creer l'executable et noter sa dependance envers le nom `libpile.so`.
3. **Chargement dynamique** : quand on execute `build/main`, le chargeur du systeme trouve et charge `libpile.so` en memoire.

Le header `.hpp` est necessaire pour compiler le programme appelant. La bibliotheque `.so` est necessaire pour l'edition de liens et doit encore etre presente quand on lance l'executable. Pour utiliser un nouveau type de `Pile<T>`, il faut l'ajouter aux instanciations explicites dans `PileGenericTabDy.cpp` et aux declarations `extern template` dans le header, puis reconstruire `libpile.so`.

## Refaire la construction complete

Depuis le dossier du projet, les commandes essentielles sont :

```sh
mkdir -p build
g++ -std=c++17 -fPIC -c PileGenericTabDy.cpp -o build/PileGenericTabDy.o
g++ -shared -o build/libpile.so build/PileGenericTabDy.o
g++ -std=c++17 -c main.cpp -o build/main.o
g++ -o build/main build/main.o -Lbuild -lpile -Wl,-rpath,'$ORIGIN'
./build/main
```

La commande `-c` demande a `g++` de produire un objet `.o` et de s'arreter avant l'edition de liens. Pour l'apprentissage, les sections precedentes separent en plus preprocessing, generation assembleur et assemblage.
