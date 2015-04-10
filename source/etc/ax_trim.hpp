#ifndef __AX_UTIL_TRIM_HPP__
#define __AX_UTIL_TRIM_HPP__

#include <algorithm>
#include <iterator>

namespace ax{
    namespace util{
        template <typename _String>
        auto trim (_String const &str_) {
            static const auto _is_space = [] (auto c) { 
                static const char space_ [] = "\t\n\f\r ";
                return std::binary_search (std::begin (space_), std::end (space_), c);
            };
            return _String (
                std::find_if_not (std::begin (str_), 
                    std::end (str_), _is_space),
                std::find_if_not (std::rbegin (str_), 
                    std::rend (str_), _is_space).base ()
            );
                
        }
    }
}

#endif