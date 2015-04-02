#include "../source/ax_sha1.hpp"
#include <cstdio>
int main () {
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