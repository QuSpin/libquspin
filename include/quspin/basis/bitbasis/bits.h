#ifndef __QUSPIN_BASIS_BITBASIS_BITS_H__
#define __QUSPIN_BASIS_BITBASIS_BITS_H__

#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "quspin/basis/types.h"
#include "quspin/basis/bitbasis/info.h"


namespace quspin::basis {


// local degrees of freedom stored in contiguous chunks of bits
template<typename I>
struct bit_set { // thin wrapper used for convience
    // doesn't allocate any data
    static const int lhss = 2;
    static constexpr I mask = 1;
    static const dit_integer_t bits = 1;

    typedef I bitset_t;
    I content;

    bit_set(const I _content) : content(_content) {}
    bit_set(const bit_set<I>& other) : content(other.content) {}

    bit_set(const std::vector<dit_integer_t>& bits_vec, const int lhss = 2){
        content = 0;
        for(size_t i=0;i<bits_vec.size();i++){
            content |= (I(bits_vec[i]) << i*bits);
        }
    }

    std::vector<dit_integer_t> to_vector(const int length=0) const {
        const int niter = (length>0 ? length : bit_info<I>::bits/bits);

        std::vector<dit_integer_t> out(niter);
        for(int i=0;i<niter;++i){
            out[i] = integer<dit_integer_t,I>::cast((content >> i*bits) & mask);
        }
        return out;
    }

    std::string to_string(const int length=0)const {
        auto bit_vec = to_vector(length);
        std::stringstream out;
        for(auto ele : bit_vec){
            out << (int)ele << " "; 
        }
        return out.str();
    }

};

template<typename I>
int get_sub_bitstring(const bit_set<I>& s,const int i){
    return integer<int,I>::cast( (s.content >> i) & I(1));
}

template<typename I,std::size_t N>
int get_sub_bitstring(const bit_set<I>& s, const std::array<int,N>& locs){
    int out = 0;
    for(int i=N-1;i>0;--i){
        out |= get_sub_bitstring(s,locs.at(i));
        out <<= 1;
    }
    out |= get_sub_bitstring(s,locs.at(0));

    return out;
}



template<typename I>
bit_set<I> set_sub_bitstring(const bit_set<I>& s,const int in,const int i){
    return  bit_set<I>(s.content ^ (( (I(in & 1) << i ) ^ s.content) & (I(1) << i)));
}

template<typename I,std::size_t N>
bit_set<I> set_sub_bitstring(const bit_set<I>& s,const int in, const std::array<int,N>& locs){
    I out = s.content;
    I in_I = I(in);
    for(const int loc : locs){
        const int shift = loc * s.bits;
        out ^= (((in_I & s.mask) << shift ) ^ s.content)  &  ( s.mask << shift );
        in_I >>= 1;
    }

    return bit_set<I>(out);
}

template<typename I>
inline bool operator<(const bit_set<I>& lhs, const bit_set<I>& rhs){return lhs.content < rhs.content;}

template<typename I>
inline bool operator>(const bit_set<I>& lhs, const bit_set<I>& rhs){return lhs.content > rhs.content;}

template<typename I>
inline bool operator==(const bit_set<I>& lhs, const bit_set<I>& rhs){return lhs.content == rhs.content;}

} // end namespace quspin::basis


#ifdef QUSPIN_UNIT_TESTS

namespace quspin::basis { // explicit instantiation for code coverage

template struct bit_set<uint8_t>;


template int get_sub_bitstring<uint8_t>(const bit_set<uint8_t>&,const int);
template int get_sub_bitstring<uint8_t,2>(const bit_set<uint8_t>&,const std::array<int,2>&);
template int get_sub_bitstring<uint8_t,4>(const bit_set<uint8_t>&,const std::array<int,4>&);

template bit_set<uint8_t> set_sub_bitstring<uint8_t>(const bit_set<uint8_t>&,const int, const int);
template bit_set<uint8_t> set_sub_bitstring<uint8_t,3>(const bit_set<uint8_t>& s,const int,const std::array<int,3>&);

template bool operator< <uint8_t>(const bit_set<uint8_t>&, const bit_set<uint8_t>&);
template bool operator> <uint8_t>(const bit_set<uint8_t>&, const bit_set<uint8_t>&);
template bool operator== <uint8_t>(const bit_set<uint8_t>&, const bit_set<uint8_t>&);

}

TEST_SUITE("quspin/basis/bitbasis/bits.h") {

    TEST_CASE("get_bit_substring") {

        using namespace quspin::basis;

        bit_set<uint8_t> state(0b01010111);

        CHECK(get_sub_bitstring(state,0) == 1);
        CHECK(get_sub_bitstring(state,3) == 0);
        CHECK(get_sub_bitstring(state,7) == 0);
        CHECK(get_sub_bitstring(state,9) == 0);

        std::array<int,2> l1 = {0,1};
        std::array<int,4> l2 = {0,6,4,3};

        CHECK(get_sub_bitstring(state,l1) == 0b11);
        CHECK(get_sub_bitstring(state,l2) == 0b0111);

    }

    TEST_CASE("set_sub_bitstring") {

        using namespace quspin::basis;

        bit_set<uint8_t> state(0b01010111);

        bit_set<uint8_t> result = set_sub_bitstring(state,0,0);
        CHECK(result.content == 0b01010110);

        result = set_sub_bitstring(state,1,1);
        CHECK(result.content == 0b01010111);

        result = set_sub_bitstring(state,0,1);
        CHECK(result.content == 0b01010101);
        
        result = set_sub_bitstring(state,1,3);
        CHECK(result.content == 0b01011111);

        result = set_sub_bitstring(state,1,5);
        CHECK(result.content == 0b01110111);

        std::array<int,3> locs1 = {3,5,7};
        result = set_sub_bitstring(state,(int)0b00000111,locs1);
        CHECK(result.content == 0b11111111);

        std::array<int,3> locs2 = {5,0,7};
        result = set_sub_bitstring(state,(int)0b00000101,locs2);
        CHECK(result.content == 0b11110110);

    }

    TEST_CASE("operators") {
        using namespace quspin::basis;

        bit_set<uint8_t> s1(0b1010111);
        bit_set<uint8_t> s2(0b1010111);
        bit_set<uint8_t> s3(0b1011111);
        bit_set<uint8_t> s4(0b1010011);


        CHECK(s1==s2);
        CHECK(s3>s1);
        CHECK(s4<s1);
    }


    TEST_CASE("to_/from_vector") {
        using namespace quspin::basis;

        bit_set<uint8_t> state(0b01010111);

        std::vector<dit_integer_t> bits = {1,1,1,0,1,0,1,0};

        CHECK(bit_set<uint8_t>(bits) == state);
        CHECK(state.to_vector() == bits);

    }

    TEST_CASE("to_string") {
        using namespace quspin::basis;

        bit_set<uint8_t> s(0b01100100);

        std::string bits = "0 0 1 0 0 1 1 0 "; // note reverse order
        
        CHECK(s.to_string() == bits);
    }
}

#endif



#endif