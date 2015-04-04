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
    auto hbfile_ = std::ofstream ("hbo.bin", std::ios::binary);
    auto nbfile_ = std::ofstream ("nbo.bin", std::ios::binary);
    auto hbstrm_ = ax::util::hbo::binary_ostream (hbfile_);
    auto nbstrm_ = ax::util::nbo::binary_ostream (nbfile_);

    std::uint64_t ui = 0;
    std::cin >> ui;

    hbstrm_ << ui;
    nbstrm_ << ui;


    //std::cin.get ();
    return 0;
}