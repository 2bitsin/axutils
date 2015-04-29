#include <etc/ax_trim.hpp>
#include <iostream>
#include <string>
int main () {
    std::string s;
    std::getline (std::cin, s);
    s = ax::util::trim (s);
    std::cout << '"' << s << '"';
    return 0;
}