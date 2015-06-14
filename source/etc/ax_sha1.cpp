#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "ax_sha1.hpp"
#include "ax_endian.hpp"
#include <cstring>
#include <algorithm>


ax::util::sha1::sha1 ()
:   mbindex_    (0u),
    digest_     ({
        0x67452301, 0xEFCDAB89, 
        0x98BADCFE, 0x10325476, 
        0xC3D2E1F0}),
    length_     (0ull),
    mfinal_     (false)
{}


std::size_t ax::util::sha1::operator () (
    void const * vdata_, 
    std::size_t len_) 
{
    if (mfinal_) throw std::runtime_error ("SHA-1: already finalized");
    auto const *data_ = reinterpret_cast<
        std::uint8_t const *>(vdata_);
    auto const *edata_ = data_ + len_;
    while (data_ < edata_) {
        auto l0_ = std::min (BUFFER_SIZE-mbindex_, 
            std::size_t (edata_-data_));
        std::copy (data_, data_+l0_, mbuffer_+mbindex_);
        length_ += (l0_<<3);
        mbindex_ += l0_;
        data_ += l0_;
        if (mbindex_ < BUFFER_SIZE)
            continue;
        _digest ();
    }
    return len_;
}

ax::util::sha1::value_type 
    ax::util::sha1::get () const 
{
    auto copy_ = *this;
    return copy_.done ();
}

template <typename _Htype>
static inline auto __to_string (_Htype &&hash_) {
    static const char format_ [] = "%08X%08X%08X%08X%08X";
    char buffer_ [48];
    auto digest_ = std::forward<_Htype> (hash_).get ();
    std::sprintf (buffer_, format_,        
        digest_ [0], digest_ [1], 
        digest_ [2], digest_ [3], 
        digest_ [4]);
    return std::string (buffer_, 40);
}

std::string ax::util::sha1::string () const {
    return __to_string (*this);
}

namespace _imp {
    template <unsigned Bits, typename Type>
        static inline Type bit_rotate_left (Type const &in) {
            auto const Size = sizeof (Type) << 3;
            return (in << Bits)|(in >> (Size - Bits));
        }
}

void ax::util::sha1::_digest () {
    auto u32buff_ = reinterpret_cast<
        std::uint32_t const (&) [16]> (mbuffer_);    
    auto w = std::array <std::uint32_t, 80> ();
    auto d = digest_;

    static auto const iterate_ = [] (auto &d, auto const &w, auto t, auto v) {
        d [4] = _imp::bit_rotate_left<5> (d [0]) + d [4] + w [t] + v;
        d [1] = _imp::bit_rotate_left<30> (d [1]);
        std::rotate (std::rbegin (d), std::rbegin (d) + 1, std::rend (d));        
    };

    for (auto t = 0; t < 16; ++t) 
        w [t] = ax::host_to_network (u32buff_ [t]);    
    for (auto t = 16; t < 80; ++t) 
        w [t] = _imp::bit_rotate_left<1> (w [t-3] ^ w [t-8] ^ w [t-14] ^ w [t-16]);  
    for (auto t = 0; t < 20; ++t)
        iterate_ (d, w, t, 0x5A827999 + ((d [1] & d [2]) | ((~d [1]) & d [3])));    
    for (auto t = 20; t < 40; ++t)
        iterate_ (d, w, t, 0x6ED9EBA1 + (d [1] ^ d [2] ^ d [3]));    
    for (auto t = 40; t < 60; ++t)
        iterate_ (d, w, t, 0x8F1BBCDC + ((d [1] & d [2]) | (d [1] & d [3]) | (d [2] & d [3])));    
    for (auto t = 60; t < 80; ++t)
        iterate_ (d, w, t, 0xCA62C1D6 + (d [1] ^ d [2] ^ d [3]));
    for (auto i = 0; i < 5; ++i)
        digest_ [i] = digest_ [i] + d [i];
    mbindex_ = 0;
}

void ax::util::sha1::_done () {
    if (mfinal_)
        return;

    mbuffer_ [mbindex_++] = 0x80;
    std::fill (mbuffer_ + mbindex_, mbuffer_ + BUFFER_SIZE, 0);
    if (mbindex_ > 56) {           
        _digest ();
        std::fill (mbuffer_, mbuffer_ + 56, 0);
    }    
    reinterpret_cast<std::uint64_t &> (mbuffer_ [56]) =
        ax::host_to_network (length_);
    _digest ();
    mfinal_ = true;
}

ax::util::sha1::value_type const &
    ax::util::sha1::done () 
{
    if (!mfinal_)
        _done ();
    return digest_;
}


