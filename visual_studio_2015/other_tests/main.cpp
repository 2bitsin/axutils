#include <etc/ax_trim.hpp>
#include <etc/ax_iconv.hpp>
#include <iostream>
#include <string>


int main () {
    using namespace ax::literals;

    std::string    s08s = u8"じゅ簨 㫨㠣盤ざびょ"_;
    std::wstring   w16s = u8"じゅ簨 㫨㠣盤ざびょ"_;
    std::u16string u16s = u8"じゅ簨 㫨㠣盤ざびょ"_;
    std::u32string u32s = u8"じゅ簨 㫨㠣盤ざびょ"_;


    auto s = ax::transcode<ax::utfw_t, ax::utf8_t> (u8"じゅ簨 㫨㠣盤ざびょ");

    return 0;
}