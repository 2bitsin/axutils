#ifndef __WTK_ENDIAN_HPP__
#define __WTK_ENDIAN_HPP__

#include <cstdint>
#include <cstdlib>
#include "ax_intrinsics.hpp"

namespace ax {
    inline namespace endian {
        template <typename _Dtype, typename _Stype>
        struct conversion {
            template <typename _Vtype> 
            static _Vtype cast (const _Vtype& x) { 
                return ax::bswap (x); 
            }
        };
        
        template <typename _Unison>
        struct conversion<_Unison, _Unison>{
            template <typename _Vtype>
            static _Vtype cast (const _Vtype& x) {
                return x;
            }
        };            

        struct little_endian_tag {};
        struct big_endian_tag {};

        typedef little_endian_tag host_byte_order_tag;
        typedef big_endian_tag network_byte_order_tag;

        template <typename _Stype, typename _Dtype, typename _Vtype>
        _Vtype endian_cast (const _Vtype& x) {
            return ::ax::endian::conversion<_Dtype, _Stype>::cast (x);
        }

        template <typename _Vtype>
        _Vtype host_to_network (const _Vtype& x) {
            return ax::endian_cast<network_byte_order_tag, host_byte_order_tag> (x);
        }

        template <typename _Vtype>
        _Vtype network_to_host (const _Vtype& x) {
            return ax::endian_cast<host_byte_order_tag, network_byte_order_tag> (x);
        }

        template <typename _Vtype>
        _Vtype host_to_le (const _Vtype& x) {
            return ax::endian_cast<little_endian_tag, host_byte_order_tag> (x);
        }

        template <typename _Vtype>
        _Vtype le_to_host (const _Vtype& x) {
            return ax::endian_cast<host_byte_order_tag, little_endian_tag> (x);
        }

        template <typename _Vtype>
        _Vtype host_to_be (const _Vtype& x) {
            return ax::endian_cast<big_endian_tag, host_byte_order_tag> (x);
        }

        template <typename _Vtype>
        _Vtype be_to_host (const _Vtype& x) {
            return ax::endian_cast<host_byte_order_tag, big_endian_tag> (x);
        }

    }

}


#endif
