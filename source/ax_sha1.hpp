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
            std::size_t operator () (void const * data_, std::size_t length_);            
            value_type get () const;   
            std::string string () const;            
            value_type const &done ();
        private:

            enum { BUFFER_SIZE = 64};

            void _digest ();
            void _done ();

            std::uint8_t    mbuffer_ [BUFFER_SIZE];
            std::uintptr_t  mbindex_;
            value_type      digest_;
            std::uint64_t   length_;
            bool            mfinal_;
        };
    }
}

#endif


