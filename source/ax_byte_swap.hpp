#ifndef __AX_BYTE_SWAP_HPP__
#define __AX_BYTE_SWAP_HPP__

#include <cstdint>

namespace ax {
    namespace util {
        namespace detail {
            template <unsigned N> struct uint_type;
            
            template <> struct uint_type<8> { typedef std::uint8_t type ; };
            template <> struct uint_type<16> { typedef std::uint16_t type ; };
            template <> struct uint_type<32> { typedef std::uint32_t type ; };
            template <> struct uint_type<64> { typedef std::uint64_t type ; };
            #if defined(_MSC_VER) && !defined(__AX_DISABLE_INTRINSICS)
                template <unsigned FullSize> 
                    struct swap;
                template <>
                    struct swap<16> {
                        typedef uint_type<16>::type type;
                        static inline type value (type const &i){
                            return _byteswap_ushort (i);
                        }
                    };
                template <>
                    struct swap<32> {
                        typedef uint_type<32>::type type;
                        static inline type value (type const &i){
                            return _byteswap_ulong (i);
                        }
                    };
                template <>
                    struct swap<64> {
                        typedef uint_type<64>::type type;
                        static inline type value (type const &i){
                            return _byteswap_uint64 (i);
                        }
                    };
            #elif defined(__GNUC__) && !defined(__AX_DISABLE_INTRINSICS)
                template <unsigned FullSize> 
                    struct swap;
                template <>
                    struct swap<16> {
                        typedef uint_type<16>::type type;
                        static inline type value (type const &i){
                            return __builtin_bswap16 (i);
                        }
                    };
                template <>
                    struct swap<32> {
                        typedef uint_type<32>::type type;
                        static inline type value (type const &i){
                            return __builtin_bswap32 (i);
                        }
                    };
                template <>
                    struct swap<64> {
                        typedef uint_type<64>::type type;
                        static inline type value (type const &i){
                            return __builtin_bswap64  (i);
                        }
                    };
            #else
                template <unsigned FullSize> 
                    struct swap {
                        static auto const HalfSize = FullSize >> 1;
                        typedef typename uint_type<FullSize>::type type;
                        typedef typename uint_type<HalfSize>::type half_type;            
                        static inline type value (type const &i) {                
                            return swap<HalfSize>::value (half_type (i >> HalfSize))|
                                (type (swap<HalfSize>::value (half_type (i))) << HalfSize);
                        }
                    };
            #endif

            template <>
                struct swap<8> {
                    typedef uint_type<8>::type type;
                    static inline type value (type const &i){
                        return i;
                    }
                };
        }

        template <typename _Type>
            typename std::enable_if<std::is_unsigned<_Type>::value, _Type>::type 
                byte_swap (_Type const &i)
            {
                return detail::swap<(sizeof (_Type) << 3)>::value (i);
            } 

        template <typename _Type> 
            typename std::enable_if<std::is_signed<_Type>::value, _Type>::type
                byte_swap (_Type const &i)
            {
                auto val_ = byte_swap (reinterpret_cast<
                    typename std::make_unsigned<_Type>::type const &> (i));
                return reinterpret_cast<_Type const &>(val_);
            } 

        template <typename _Type>
            typename std::remove_reference<_Type>::type
                host_to_network_byte_order (_Type &&in) 
            {
                #ifdef __AX_BIG_ENDIAN__
                return in;
                #else
                return byte_swap (std::forward<_Type> (in));
                #endif
            }

        template <typename _Type>
            typename std::remove_reference<_Type>::type
                network_to_host_byte_order (_Type &&in) 
            {
                #ifdef __AX_BIG_ENDIAN__
                return in;
                #else
                return byte_swap (std::forward<_Type> (in));
                #endif
            }
    }
}

#endif