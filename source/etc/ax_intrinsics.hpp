#ifndef __WTK_INTRINSICS_HPP__
#define __WTK_INTRINSICS_HPP__

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace ax {
    inline namespace endian {
        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint8_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return x;
        }

        #if defined(_MSC_VER)

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint16_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (_byteswap_ushort (
                reinterpret_cast<const std::uint16_t&> (x)));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint32_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (_byteswap_ulong (
                reinterpret_cast<const std::uint32_t&> (x)));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint64_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (_byteswap_uint64 (
                reinterpret_cast<const std::uint64_t&> (x)));
        }


        #elif defined(__GNUC__)


        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint16_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (__builtin_bswap16 (
                reinterpret_cast<const std::uint16_t&> (x)));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint32_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (__builtin_bswap32 (
                reinterpret_cast<const std::uint32_t&> (x)));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint64_t), _Vtype>::type
            bswap (const _Vtype& x) {
            return std::forward<_Vtype&&> (__builtin_bswap64 (
                reinterpret_cast<const std::uint64_t&> (x)));
        }

        #else 

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint16_t), _Vtype>::type
            bswap (const _Vtype& x) {
            const auto& xr = reinterpret_cast<const std::uint16_t&> (x);
            return std::forward<_Vtype&&> ((xr >> 8) | (xr << 8));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint32_t), _Vtype>::type
            bswap (const _Vtype& x) {
            const auto& xr = reinterpret_cast<const std::uint32_t&> (x);
            return std::forward<_Vtype&&> (bswap<std::uint16_t> (xr >> 16) |
                (static_cast<std::uint32_t> (bswap<std::uint16_t> (xr & 0xffff)) << 16));
        }

        template <typename _Vtype>
        inline typename std::enable_if<sizeof (_Vtype) == sizeof (std::uint64_t), _Vtype>::type
            bswap (const _Vtype& x) {
            const auto& xr = reinterpret_cast<const std::uint64_t&> (x);
            return std::forward<_Vtype&&> (bswap<std::uint32_t> (xr >> 32) |
                (static_cast<std::uint64_t> (bswap<std::uint32_t> (xr & 0xffffffff)) << 32));
        }
        #endif
    }

    inline namespace bits {

        #if defined(_MSC_VER)

        template <typename _What>        
        inline typename std::enable_if<sizeof (_What) == 8, int>::type
        bit_scan_forward (const _What& w) {
            unsigned long o = 0u;
            return _BitScanForward64 (&o, reinterpret_cast<
                const std::uint64_t&> (w)) ? int (o) : -1;
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 4, int>::type
        bit_scan_forward (const _What& w) {
            unsigned long o = 0u;
            return _BitScanForward (&o, reinterpret_cast<
                const std::uint32_t&> (w)) ? int (o) : -1;
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) < 4, int>::type
        bit_scan_forward (const _What& w) {
            return bit_scan_forward (std::uintptr_t (w));
        }

        ////////////////////////////////

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 8, int>::type
        bit_scan_reverse (const _What& w) {
            unsigned long o = 0u;
            return _BitScanReverse64 (&o, reinterpret_cast<
                const std::uint64_t&> (w)) ? int (o) : -1;
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 4, int>::type
        bit_scan_reverse (const _What& w) {
            unsigned long o = 0u;
            return _BitScanReverse (&o, reinterpret_cast<
                const std::uint32_t&> (w)) ? int (o) : -1;
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) < 4, int>::type
        bit_scan_reverse (const _What& w) {
            return bit_scan_reverse (std::uintptr_t (w));
        }

        ////////////////////////////////

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 8, _What>::type
        count_leading_zeros (const _What& w) {
            return __lzcnt64 (reinterpret_cast<const std::uint64_t&> (w));
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 4, _What>::type
        count_leading_zeros (const _What& w) {
            return __lzcnt (reinterpret_cast<const std::uint32_t&> (w));
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) == 2, _What>::type
        count_leading_zeros (const _What& w) {
            return __lzcnt64 (reinterpret_cast<const std::uint16_t&> (w));
        }

        template <typename _What>
        inline typename std::enable_if<sizeof (_What) < 2, _What>::type
        count_leading_zeros (const _What& w) {
            const auto fixup_ = (sizeof (std::uintptr_t) - sizeof (_What))*8;
            return _What (count_leading_zeros (std::uintptr_t (w)) - fixup_);
        }

        #else

        #endif

    }
}

#endif
