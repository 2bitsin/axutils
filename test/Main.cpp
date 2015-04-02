#include "../source/ax_sha1.hpp"
#include "../source/ax_raw_ostream.hpp"
#include "../source/ax_byte_swap.hpp"
#include <cstdio>
#include <vector>
#include <iostream>
int main () {
    std::string buffer_;
    std::vector<std::uint8_t> vector_;
    std::ofstream out_ ("hello.bin", std::ios::binary);
    auto ros0_ = ax::util::raw_ostream (buffer_);
    auto ros1_ = ax::util::raw_ostream (vector_);

    double x;
    std::cin >> x;
    std::printf ("%016llx\n", *reinterpret_cast<std::uint64_t const*> (&x));
    x = ax::util::host_to_network_byte_order (x);
    std::printf ("%016llx\n", *reinterpret_cast<std::uint64_t const*> (&x));
    x = ax::util::network_to_host_byte_order (x);
    std::printf ("%016llx\n", *reinterpret_cast<std::uint64_t const*> (&x));
    std::getchar ();

    ax::util::sha1 digest_;
    digest_ (
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD"
        "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD",
        256
    );
    
    auto val = digest_.get ();
    
    std::printf ("%08X%08X%08X%08X%08X\n", val [0], val [1], val [2], val [3], val [4]);
    std::getchar ();
    return 0;
}