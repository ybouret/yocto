#ifndef YOCTO_SORT_YSORT_INCLUDED
#define YOCTO_SORT_YSORT_INCLUDED 1

#include "yocto/comparator.hpp"
#include "yocto/code/bswap.hpp"
#include "yocto/sort/network.hpp"

namespace yocto
{
    inline void __echanger(int tableau[], int a, int b)
    {
        int temp = tableau[a];
        tableau[a] = tableau[b];
        tableau[b] = temp;
    }

    inline void __ySort(int tableau[], int debut, int fin)
    {
        int gauche = debut-1;
        int droite = fin+1;
        const int pivot = tableau[debut];

        /* Si le tableau est de longueur nulle, il n'y a rien à faire. */
        if(debut >= fin)
            return;

        /* Sinon, on parcourt le tableau, une fois de droite à gauche, et une
         autre de gauche à droite, à la recherche d'éléments mal placés,
         que l'on permute. Si les deux parcours se croisent, on arrête. */
        while(1)
        {
            do droite--; while(tableau[droite] > pivot);
            do gauche++; while(tableau[gauche] < pivot);

            if(gauche < droite)
                __echanger(tableau, gauche, droite);
            else break;
        }

        /* Maintenant, tous les éléments inférieurs au pivot sont avant ceux
         supérieurs au pivot. On a donc deux groupes de cases à trier. On utilise
         pour cela... la méthode quickSort elle-même ! */
        __ySort(tableau, debut, droite);
        __ySort(tableau, droite+1, fin);
    }

    template <typename T,typename FUNC>
    inline void _ySort(T        *tableau,
                       FUNC     &cmp,
                       const int debut,
                       const int fin ) throw()
    {
        if(debut>=fin) return;

        register int       gauche = debut;
        register int       droite = fin;
        register const int objets = fin - (--gauche);

        switch(objets)
        {
            case 2:
                __netsort<T,FUNC>::op2(tableau[debut],tableau[fin],cmp);
                return;

            case 3:
                __netsort<T,FUNC>::op3(tableau[debut],tableau[debut+1],tableau[fin],cmp);
                return;

            case 4:
                __netsort<T,FUNC>::op4(tableau[debut],tableau[debut+1],tableau[debut+2],tableau[fin],cmp);
                return;

            case 6:
                __netsort<T,FUNC>::op6(tableau[debut],tableau[debut+1],tableau[debut+2],tableau[debut+3],tableau[debut+4],tableau[fin],cmp);
                return;

            default: {
                ++droite;
                uint64_t     tmp[YOCTO_U64_FOR_ITEM(T)];
                T           &pivot  = *(T*)&tmp[0];
                core::bmove<sizeof(T)>(&pivot,&tableau[debut]);

                while(true)
                {
                    do droite--; while( cmp(pivot,tableau[droite]) < 0 );
                    do gauche++; while( cmp(tableau[gauche],pivot) < 0 );
                    if(gauche<droite)
                    {
                        core::bswap<sizeof(T)>(&tableau[gauche],&tableau[droite]);
                    }
                    else
                    {
                        break;
                    }
                }
                _ySort(tableau,cmp,debut,droite);
                _ySort(tableau,cmp,++droite,fin);
            }
        }
    }

    template <typename T,typename FUNC>
    inline void ySort(array<T> &arr, FUNC &compare) throw()
    {
        const size_t n = arr.size();
        if(n>1)
        {
            _ySort(&arr[1],compare,0,n-1);
        }
    }

}

#endif

