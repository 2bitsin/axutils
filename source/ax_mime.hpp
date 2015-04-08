#ifndef __AX_MIME_HPP__
#define __AX_MIME_HPP__

#include <string>

namespace ax {
    namespace util {
        struct mime_type {
            static mime_type const &get (std::string const &ext);
            operator std::string const & () const;
            operator char const * () const;
            std::string const &string () const;
        private:
            template <typename _Type>
            mime_type (_Type &&in_):
                string_ (std::forward<_Type> (in_))
            {}
            std::string string_;
        };
    }
}

#endif
