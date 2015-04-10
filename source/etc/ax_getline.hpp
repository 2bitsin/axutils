#ifndef __AX_UTIL_GETLINE_HPP__
#define __AX_UTIL_GETLINE_HPP__

#include "ax_to_string.hpp"
#include <array>

namespace ax{
    namespace util{
        template <typename _Stream, typename _String, typename _Ctype = _String::value_type>
        inline bool getline (_Stream &stream_, _String &line_, const _String &delim_ = 
            ax::util::to_string<typename _String::value_type> ("\r\n")) 
        {
            std::array<_Ctype, 2048> temp_;
            line_ = _String ();
            auto avail_ = decltype (stream_.peek (temp_.data (), temp_.size ())) {};
            while ((avail_ = stream_.peek (temp_.data (), temp_.size ())) > 0) {  
                auto sbuff_ = _String (temp_.data (), avail_);
                auto stpos_ = sbuff_.find (delim_);
                if (stpos_ != sbuff_.npos) {
                    auto trunc_ = stpos_ + delim_.size ();
                    line_.append (temp_.data (), stpos_);
                    stream_.ignore (trunc_);
                    return true;
                }
                stream_.ignore (avail_);
                line_.append (temp_.data (), avail_);                
            }
            return false;
        }
    }
}

#endif
