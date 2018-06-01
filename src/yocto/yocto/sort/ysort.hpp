#ifndef YOCTO_SORT_YSORT_INCLUDED
#define YOCTO_SORT_YSORT_INCLUDED 1

#include "yocto/comparator.hpp"
#include "yocto/sort/nwsrt.hpp"
#include "yocto/code/bmove.hpp"
#include "yocto/sequence/lw-array.hpp"

namespace yocto
{
#define Y_SORT_IMPL(N)  case N: nwsrt<T>::op##N(&tableau[debut],cmp); break
#define Y_SORT_IMPL2(N) case N: nwsrt<T>::co_op##N(&arr[debut],&brr[debut],cmp); break
#define Y_SORT_REPEAT(MACRO) \
MACRO( 2); MACRO( 3); MACRO( 4); MACRO( 5); MACRO( 6); MACRO( 7); MACRO( 8); \
MACRO( 9); MACRO(10); MACRO(11); MACRO(12); MACRO(13); MACRO(14); MACRO(15); \
MACRO(16); MACRO(17); MACRO(18); MACRO(19); MACRO(20); MACRO(21); MACRO(22); \
MACRO(23); MACRO(24)

    template <typename T,typename FUNC>
    inline void _ySort(T        *tableau,
                       FUNC     &cmp,
                       const int debut,
                       const int fin )
    {
        if(debut>=fin) return;

        int       gauche = debut;
        int       droite = fin;
        const int objets = fin - (--gauche);

        switch(objets)
        {
                Y_SORT_REPEAT(Y_SORT_IMPL);

            default: {
                const T pivot(tableau[debut]);
                ++droite;

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

    template <typename T,typename U,typename FUNC>
    inline void _ySort2(T        *arr,
                        U        *brr,
                        FUNC     &cmp,
                        const int debut,
                        const int fin ) 
    {
        if(debut>=fin) return;

        int       gauche = debut;
        int       droite = fin;
        const int objets = fin - (--gauche);

        switch(objets)
        {
                Y_SORT_REPEAT(Y_SORT_IMPL2);

            default: {
                const T pivot(arr[debut]);

                ++droite;
                while(true)
                {
                    do droite--; while( cmp(pivot,arr[droite]) < 0 );
                    do gauche++; while( cmp(arr[gauche],pivot) < 0 );
                    if(gauche<droite)
                    {
                        core::bswap<sizeof(T)>(&arr[gauche],&arr[droite]);
                        core::bswap<sizeof(U)>(&brr[gauche],&brr[droite]);
                    }
                    else
                    {
                        break;
                    }
                }
                _ySort2(arr,brr,cmp,debut,droite);
                _ySort2(arr,brr,cmp,++droite,fin);
            }
        }
    }

    template <typename T, typename U, typename FUNC>
    inline void yCoSort(array<T> &arr, array<U> &brr, FUNC &compare) throw()
    {
        assert(arr.size()==brr.size());
        const size_t n = arr.size();
        if(n>1)
        {
            _ySort2(&arr[1],&brr[1],compare,0,n-1);
        }
    }

    //! C-style sorting
    template<typename T,typename FUNC>
    inline void ySort( T *a, size_t n, FUNC &compare )
    {
        lw_array<T> A(a,n);
        ySort<T,FUNC>(A,compare);
    }

}

#endif

