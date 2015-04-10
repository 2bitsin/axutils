#ifndef __AX_UTIL_TO_STRING_HPP__
#define __AX_UTIL_TO_STRING_HPP__

#include <iterator>
#include <string>
#include <sstream>

namespace ax {
    namespace util {
        template <typename _Ctype = char, typename _Source>
        std::basic_string<_Ctype> to_string (_Source &&source_) {
            std::basic_stringstream<_Ctype> stream_;
            stream_ << std::forward<_Source> (source_);
            return stream_.str ();
        }
    }
}

#endif
