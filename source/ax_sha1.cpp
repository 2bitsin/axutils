#include "ax_sha1.hpp"
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


ax::util::sha1 &
    ax::util::sha1::operator()(
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
        digest ();
    }
    return *this;
}

ax::util::sha1::value_type const &
    ax::util::sha1::get () 
{
    if (!mfinal_)
        complete ();
    return digest_;
}

ax::util::sha1::value_type const &
    ax::util::sha1::get () const 
{
    if (!mfinal_)
        throw std::logic_error ("SHA-1: Cannot finalize immutable object");
    return digest_;
}

namespace _imp {
    template <unsigned Bits, typename Type>
        static inline Type bit_rotate_left (Type const &in) {
            auto const Size = sizeof (Type) << 3;
            return (in << Bits)|(in >> (Size - Bits));
        }

    template <unsigned N> struct ui;

    template <> struct ui<8> { typedef std::uint8_t type ; };
    template <> struct ui<16> { typedef std::uint16_t type ; };
    template <> struct ui<32> { typedef std::uint32_t type ; };
    template <> struct ui<64> { typedef std::uint64_t type ; };

    template <unsigned FullSize> 
        struct swap {
            static auto const HalfSize = FullSize >> 1;
            typedef typename ui<FullSize>::type type;
            typedef typename ui<HalfSize>::type half_type;            
            static inline type value (type const &i) {                
                return swap<HalfSize>::value (half_type (i >> HalfSize))|
                    (type (swap<HalfSize>::value (half_type (i))) << HalfSize);
            }
        };
    template <>
        struct swap<8> {
            typedef ui<8>::type type;
            static inline type value (type const &i){
                return i;
            }
        };

    template <typename Type>
        Type byte_swap (Type const &i){
            return swap<(sizeof (Type) << 3)>::value (i);
        } 
}

void ax::util::sha1::digest () {

    typedef std::uint32_t (*xop_t) (
        std::uint32_t d1, 
        std::uint32_t d2, 
        std::uint32_t d3);

    static xop_t const xop_ [4] = {
        [] (std::uint32_t d1, std::uint32_t d2, std::uint32_t d3) { 
            return (d1 & d2) | ((~d1) & d3) ; },
        [] (std::uint32_t d1, std::uint32_t d2, std::uint32_t d3) { 
            return d1 ^ d2 ^ d3 ; },
        [] (std::uint32_t d1, std::uint32_t d2, std::uint32_t d3) { 
            return (d1 & d2) | (d1 & d3) | (d2 & d3) ; },
        xop_ [1]
    };

    auto u32buff_ = reinterpret_cast<
        std::uint32_t const (&) [16]> (mbuffer_);    
    static const std::uint32_t k [] = {
        0x5A827999, 0x6ED9EBA1, 
        0x8F1BBCDC, 0xCA62C1D6};        
    auto w = std::array <std::uint32_t, 80> ();
    auto d = digest_;

    for (auto t = 0; t < 80; ++t) {     
        auto const s = t / 20;
        w [t] = t >= 16 ? _imp::bit_rotate_left<1> (
            w [t-3] ^ w [t-8] ^ w [t-14] ^ w [t-16]):
            _imp::byte_swap (u32buff_ [t]);
        d [4] = xop_ [s] (d [1], d [2], d [3]) + 
            _imp::bit_rotate_left<5>(d [0]) + 
            d [4] + w [t] + k [s];            
        d [1] = _imp::bit_rotate_left<30> (d [1]);            
        std::rotate (std::rbegin (d), 
            std::rbegin (d) + 1, 
            std::rend (d));        
    }
    for (auto i = 0; i < 5; ++i)
        digest_ [i] += d [i];

    mbindex_ = 0;
}

void ax::util::sha1::complete () {
    if (mfinal_)
        return;

    mbuffer_ [mbindex_++] = 0x80;
    std::fill (mbuffer_ + mbindex_, mbuffer_ + BUFFER_SIZE, 0);
    if (mbindex_ > 56) {           
        digest ();
        std::fill (mbuffer_, mbuffer_ + 56, 0);
    }    
    reinterpret_cast<std::uint64_t &> (mbuffer_ [56]) =
        _imp::byte_swap (length_);
    digest ();
    mfinal_ = true;
}


