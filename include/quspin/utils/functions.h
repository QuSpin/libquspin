#ifndef __QUSPIN_UTILS_FUNCTIONS_H__
#define __QUSPIN_UTILS_FUNCTIONS_H__

#include <vector>
#include <complex>

namespace quspin {

template<size_t base,size_t N>
struct integer_pow {
    enum pow : size_t {value = base * static_cast<size_t>(integer_pow<base,N-1>::value)};
};

template<size_t base>
struct integer_pow<base,0u>{
    enum pow : size_t {value = 1};
};

}


#ifdef USE_STD_COMPLEX

#include <complex>

namespace quspin {
    template<class T>
    std::complex<T> conj(const std::complex<T>&A){return std::conj(A);}

    template<class T>
    T conj(const T& A){return A;}

    template<class T>
    T real(const std::complex<T>&A){return A.real();}

    template<class T>
    T real(const T& A){return A;}

}
#endif

#ifdef QUSPIN_UNIT_TESTS

TEST_SUITE("quspin/utils/functions.h"){
    using namespace quspin;

    TEST_CASE("integer_pow"){
        CHECK(integer_pow<2,4>::value == 16);
        CHECK(integer_pow<3,3>::value == 27);
        CHECK(integer_pow<10,3>::value == 1000);
        CHECK(integer_pow<5,5>::value == 3125);
    }
}



#endif

#endif