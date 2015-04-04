#include "../source/ax_sha1.hpp"
#include "../source/ax_binary_ostream.hpp"
#include "../source/ax_byte_swap.hpp"
#include <cstdio>
#include <vector>
#include <iostream>
int main () {    
    static const char messag_ [] = {
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 
        'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 
        'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 
        'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D'
    };
    auto digest_ = std::ofstream ("out.bin", std::ios::binary);
    auto stream_ = ax::util::hbo::binary_ostream (digest_);        
    auto hello_ = "Hello";
    auto u8hello_ = u8"Hello";
    auto Lhello_ = L"Hello";
    auto u16hello_ = u"Hello";
    auto u32hello_ = U"Hello";

    

    stream_ 
        << "Hello" << u8"Hello" << L"Hello" << u"Hello" << U"Hello"
        << hello_ << u8hello_ << Lhello_ << u16hello_ << u32hello_ 
        << std::int8_t {-1}
        << std::uint8_t {0}
        << std::int16_t {-1}
        << std::uint16_t {0}
        << std::int32_t {-1}
        << std::uint32_t {0}
        << std::int64_t {-1}
        << std::uint64_t {0}
        << -1 << 0u << -1l << 0ul << -1ll << 0ull
        << 0.9999f << 0.9999 << true;
        

    //std::cin.get ();
    return 0;
}