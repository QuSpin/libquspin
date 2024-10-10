#ifndef __QUSPIN_BASIS_SYMMETRY_H__
#define __QUSPIN_BASIS_SYMMETRY_H__


#include <vector>
#include <algorithm>
#include <cassert>
#include "quspin/basis/types.h"
#include "quspin/basis/bitbasis/utils.h"
#include "quspin/basis/bitbasis/dits.h"
#include "quspin/basis/bitbasis/bits.h"
#include "quspin/basis/bitbasis/benes.h"
#include "quspin/utils/functions.h"


namespace quspin::basis {

template <typename I>
class dit_perm // permutation of dit locations
{
private:
    benes::tr_benes<I> benes;

public:
    dit_perm(const int lhss,const std::vector<int>& perm) {
        
        // number of bits to store lhss worth of data:
        const dit_integer_t bits = constants::bits[lhss];

        assert(perm.size() <= bit_info<I>::bits/bits);

        benes::ta_index<I> index;
        for(int i=0;i<bit_info<I>::bits;i++){index[i] = benes::no_index;}

        // permute chucks of bits of length 'bits' 
        for(size_t i=0;i<perm.size();i++){
            const int dst = perm[i];
            const int src = i;
            for(int j=0;j<bits;j++){
                const int srcbit = bits * src + j;
                const int dstbit = bits * dst + j;
                index[srcbit] = dstbit;
            }   
        }

        benes::gen_benes(&benes,index);

    }
    dit_perm(const dit_perm<I>& other) : benes(other.benes) {}

    ~dit_perm() {}

    template<typename T>
    inline dit_set<I> app(const dit_set<I>& s, T& coeff) const { 
        return dit_set<I>(benes::benes_fwd(&benes,s.content),s.lhss,s.mask,s.bits); 
    }

    template<typename T>
    inline dit_set<I> inv(const dit_set<I>& s, T& coeff) const { 
        return dit_set<I>(benes::benes_bwd(&benes,s.content),s.lhss,s.mask,s.bits);  
    }
};

template <typename I>
class bit_perm // permutation of bit locations
{
private:
    benes::tr_benes<I> benes;

public:
    bit_perm(const std::vector<int>& perm) 
    {
        assert(perm.size() <= bit_info<I>::bits);

        benes::ta_index<I> index;
        for(int i=0;i<bit_info<I>::bits;i++){index[i] = benes::no_index;}

        // benes permutation is agnostic to lhss
        for(size_t i=0;i<perm.size();i++){
            index[i] = perm[i];  
        }

        benes::gen_benes(&benes,index);

    }
    bit_perm(const bit_perm<I>& other) : benes(other.benes) {}

    ~bit_perm() {}

    template<typename T>
    inline bit_set<I> app(const bit_set<I>& s, T& coeff) const { 
        return  bit_set<I>(benes::benes_fwd(&benes,s.content)); 
    }

    template<typename T>
    inline bit_set<I> inv(const bit_set<I>& s, T& coeff) const { 
        return bit_set<I>(benes::benes_bwd(&benes,s.content)); 
    }
};

// potentially change names
template <typename I>
class perm_dit // permutations of the dit states locally
{
private:
    const int lhss;
    std::vector<int> perm;
    std::vector<int> inv_perm;
    std::vector<int> locs;

public:
    perm_dit(const int _lhss, const std::vector<std::vector<int>>& _perm,const std::vector<int>& _locs) : lhss(_lhss)
    { 

        assert(_perm.size() == _locs.size());

        locs.insert(locs.end(),_locs.begin(),_locs.end());

        for(size_t i=0;i<locs.size();++i){
            const std::vector<int> p = _perm.at(i);
            std::vector<int> ip(p.size());
            assert(p.size() == lhss);

            int j = 0;
            for(const int ele : p){ip[ele] = j++;}

            perm.insert(perm.end(),p.begin(),p.end());
            inv_perm.insert(inv_perm.end(),ip.begin(),ip.end());
        }
    }
    perm_dit(const perm_dit<I>& other): lhss(other.lhss) {
        locs.insert(locs.end(),other.locs.begin(),other.locs.end());
        perm.insert(perm.end(),other.perm.begin(),other.perm.end());
        inv_perm.insert(inv_perm.end(),other.inv_perm.begin(),other.inv_perm.end());
    }
    ~perm_dit() {}

    template<typename T>
    dit_set<I> app(const dit_set<I>& s, T& coeff) const {
        
        dit_set<I> r(s);
        
        const int * perm_ptr = perm.data();

        for(const int loc : locs){
            const int bits = get_sub_bitstring(s,loc);
            r = set_sub_bitstring(r,perm_ptr[bits],loc);
            perm_ptr += lhss;
        }

        return r;
    }

    template<typename T>
    dit_set<I> inv(const dit_set<I>& s, T& coeff) const {
        dit_set<I> r(s);
        
        const int * perm_ptr = inv_perm.data();

        for(const int loc : locs){
            const int bits = get_sub_bitstring(s,loc);
            r = set_sub_bitstring(r,perm_ptr[bits],loc);
            perm_ptr += lhss;
        }
        
        return r;
    }
};

template <typename I>
class perm_bit // permutations of the bit states locally
{
private:
    const I mask; // which bits to flip

public:
    perm_bit(const I _mask) : mask(_mask) { }
    perm_bit(const std::vector<int>& mask_vec) : mask(bit_set<I>(mask_vec).content) {}
    ~perm_bit() {}

    template<typename T>
    inline bit_set<I> app(const bit_set<I>& s, T& coeff) const {

        return  bit_set<I>( s.content^mask );
    }

    template<typename T>
    inline bit_set<I> inv(const bit_set<I>& s, T& coeff) const {
        return bit_set<I>( s.content^mask );
    }
};

template<typename lat_perm_t,typename loc_perm_t,typename dits_or_bits,typename T>
class symmetry
{
private:
    std::vector<lat_perm_t> lat_symm;
    std::vector<loc_perm_t> loc_symm;
    std::vector<T> lat_char;
    std::vector<T> loc_char;

public:
    symmetry(const std::vector<lat_perm_t> &_lat_symm,const std::vector<T> &_lat_char,
        const std::vector<loc_perm_t> &_loc_symm,const std::vector<T> &_loc_char)
    {
        assert(_lat_symm.size() == _lat_char.size());
        assert(_loc_symm.size() == _loc_char.size());

        for(size_t i=0;i<lat_symm.size();i++){
            lat_symm.emplace_back(_lat_symm[i]);
            lat_char.emplace_back(_lat_char[i]);
            loc_symm.emplace_back(_loc_symm[i]);
            loc_char.emplace_back(_loc_char[i]);
        }
    }

    symmetry(const symmetry<lat_perm_t,loc_perm_t,dits_or_bits,T>& other) :
    symmetry(other.lat_symm,other.lat_char,other.loc_symm,other.loc_char) {}
    ~symmetry() {}

    size_t size() const {return lat_symm.size() * loc_symm.size();}

    std::pair<dits_or_bits,T> get_refstate(const dits_or_bits &s) const {

        dits_or_bits ss(s);
        T sign = T(1);
        T coeff = T(1);
        
        for(size_t i=0;i<loc_symm.size();++i) 
        {
            const auto r = loc_symm[i].app(s,sign);
            for(size_t j=0;j<lat_symm.size();++j)
            {
                const auto rr = lat_symm[i].app(r,sign);

                if(rr > ss){
                    ss = rr;
                    coeff = sign * lat_char[j] * loc_char[i];
                }
            }
        }

        return std::make_pair(ss,coeff);
    }

    std::pair<dits_or_bits,double> calc_norm(const dits_or_bits &s) const {
        double norm = 0.0;
        T sign = T(1);


        dits_or_bits ss(s);

        for(size_t i=0;i<loc_symm.size();++i) 
        {
            const auto r = loc_symm[i].app(s,sign);
            for(size_t j=0;j<lat_symm.size();++j)
            {
                const auto rr = lat_symm[i].app(r,sign);
                if(rr == s) norm += real(sign * lat_char[j] * loc_char[i]);
                else if(rr > ss){ss = rr;}
            }
        }
        return std::make_pair(ss,norm);
    }

    double check_refstate(const dits_or_bits &s) const {
        double norm=0.0;
        T sign = T(1);

        for(size_t i=0;i<loc_symm.size();++i) 
        {
            const auto r = loc_symm[i].app(s,sign);
            for(size_t j=0;j<lat_symm.size();++j)
            {
                const auto rr = lat_symm[i].app(r,sign);

                if(rr >  s){return std::numeric_limits<double>::quiet_NaN();};
                if(rr == s) norm += real(sign * lat_char[j] * loc_char[i]);
            }
        }

        return norm;
    }

};

}



#ifdef QUSPIN_UNIT_TESTS


namespace quspin::basis { // test cases

template class dit_perm<uint8_t>;
template dit_set<uint8_t> dit_perm<uint8_t>::app<double>(const dit_set<uint8_t>&, double&) const;
template dit_set<uint8_t> dit_perm<uint8_t>::inv<double>(const dit_set<uint8_t>&, double&) const;

template class perm_dit<uint8_t>;
template dit_set<uint8_t> perm_dit<uint8_t>::app<double>(const dit_set<uint8_t>&, double&) const;
template dit_set<uint8_t> perm_dit<uint8_t>::inv<double>(const dit_set<uint8_t>&, double&) const;

template class bit_perm<uint8_t>;
template bit_set<uint8_t> bit_perm<uint8_t>::app<double>(const bit_set<uint8_t>&, double&) const;
template bit_set<uint8_t> bit_perm<uint8_t>::inv<double>(const bit_set<uint8_t>&, double&) const;

template class perm_bit<uint8_t>;
template bit_set<uint8_t> perm_bit<uint8_t>::app<double>(const bit_set<uint8_t>&, double&) const;
template bit_set<uint8_t> perm_bit<uint8_t>::inv<double>(const bit_set<uint8_t>&, double&) const;

template class symmetry<bit_perm<uint8_t>,perm_bit<uint8_t>,bit_set<uint8_t>,double>;


}

TEST_SUITE("quspin/basis/symmetry.h"){
    #include <memory>
    using namespace quspin::basis;

    TEST_CASE("bit_perm"){
        std::vector<int> perm = {1,3,2,0};
        std::vector<int> inv = {3,0,2,1};
        bit_set<uint8_t> bit_state({0,1,0,1,1,1,0,0});
        bit_perm<uint8_t> bp(perm);
        double coeff = 1.0;

        auto result = bp.app(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 1 0 0 1 1 0 0 ");

        result = bp.inv(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 0 0 1 1 1 0 0 ");

        result = bp.app(bp.inv(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

        result = bp.inv(bp.app(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

    }

    TEST_CASE("dit_perm"){
        std::vector<int> perm = {1,3,2,0};
        std::vector<int> inv = {3,0,2,1};
        dit_set<uint8_t> dit_state({0,1,2,0},3);
        dit_perm<uint8_t> dp(3,perm);
        double coeff = 1.0;

        auto result = dp.app(dit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 0 2 0 ");

        result = dp.inv(dit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 0 2 1 ");

        result = dp.app(dp.inv(dit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 2 0 ");

        result = dp.inv(dp.app(dit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 2 0 ");

    }

    TEST_CASE("perm_bit"){
        const uint8_t mask = 0b00110101;
        std::unique_ptr<perm_bit<uint8_t>> pb = std::make_unique<perm_bit<uint8_t>>(mask);
        bit_set<uint8_t> bit_state({0,1,0,1,1,1,0,0});
        double coeff = 1.0;

        auto result = pb->app(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 1 1 1 0 0 0 0 ");

        result = pb->inv(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 1 1 1 0 0 0 0 ");

        result = pb->app(pb->inv(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

        result = pb->inv(pb->app(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

        std::vector<int> mask_vec = {1,0,1,0,1,1,0,0};
        pb = std::make_unique<perm_bit<uint8_t>>(mask_vec);

        result = pb->app(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 1 1 1 0 0 0 0 ");

        result = pb->inv(bit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "1 1 1 1 0 0 0 0 ");

        result = pb->app(pb->inv(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

        result = pb->inv(pb->app(bit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 0 1 1 1 0 0 ");

    }

    TEST_CASE("perm_dit"){
        std::vector<int> perm = {1,2,0};
        std::vector<int> inv  = {2,0,1};
        dit_set<uint8_t> dit_state({0,1,2,0},3);
        perm_dit<uint8_t> dp(3,{perm, perm}, {1,3});
        double coeff = 1.0;

        auto result = dp.app(dit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 2 2 1 ");

        result = dp.inv(dit_state,coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 0 2 2 ");

        result = dp.app(dp.inv(dit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 2 0 ");

        result = dp.inv(dp.app(dit_state,coeff),coeff);
        CHECK(coeff == 1.0);
        CHECK(result.to_string() == "0 1 2 0 ");

    }
   
}



#endif

#endif