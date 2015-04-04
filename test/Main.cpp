#include "../source/ax_sha1.hpp"
#include "../source/ax_binary_ostream.hpp"
#include "../source/ax_byte_swap.hpp"
#include <cstdio>
#include <vector>
#include <iostream>
#include <numeric>

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

    std::size_t ui;
    std::cin >> ui;

    auto vect_ = std::vector<std::uint32_t> (ui);
    std::iota (std::begin (vect_), std::end (vect_), 10000);


    hbstrm_ << vect_;
    nbstrm_ << vect_;


    //std::cin.get ();
    return 0;
}