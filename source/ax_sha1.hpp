#ifndef __AX_SHA1_HPP__
#define __AX_SHA1_HPP__

#include <cstdint>
#include <array>
#include <string>

namespace ax {
    namespace util {
        struct sha1 {
            typedef std::array<std::uint32_t, 5> value_type;
            sha1 ();
            sha1 &operator () (void const * data_, std::size_t length_);
            value_type const &get ();
            value_type const &get () const;            
        private:

            enum { BUFFER_SIZE = 64};

            void digest ();
            void complete ();

            std::uint8_t    mbuffer_ [BUFFER_SIZE];
            std::uintptr_t  mbindex_;
            value_type      digest_;
            std::uint64_t   length_;
            bool            mfinal_;
        };
    }
}

#endif


