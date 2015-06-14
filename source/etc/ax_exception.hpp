#ifndef __WTK_EXCEPTION_HPP__
#define __WTK_EXCEPTION_HPP__

namespace ax {
    inline namespace exception {
        struct bad_code_point {
            template <typename _Ctype>
            bad_code_point (_Ctype&&) 
            {}
        };
    }
}

#endif