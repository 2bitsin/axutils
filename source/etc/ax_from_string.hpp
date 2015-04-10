#ifndef __AX_UTIL_FROM_STRING_HPP__
#define __AX_UTIL_FROM_STRING_HPP__

namespace ax{
    namespace util{
        template <typename _Target, typename _Ctype> 
        _Target from_string (std::basic_string<_Ctype> const &str_) {
            _Target ret_;
            std::basic_stringstream<_Ctype> stream_ (str_);
            stream_ >> ret_;
            return ret_;
        }
    }
}

#endif