#ifndef __WTK_ICONV_HPP__
#define __WTK_ICONV_HPP__

#include <cstdint>
#include <string>
#include <type_traits>

#include "ax_intrinsics.hpp"
#include "ax_endian.hpp"
#include "ax_exception.hpp"

namespace ax {
    namespace detail {

        template <std::size_t _Bits, typename _Ctype, typename _ByteOrderTag>
        struct ucs {
            typedef _Ctype char_type;
            static const std::size_t max_chars = 1;
            static const std::size_t bom_size = 1;
        protected:
            typedef ax::conversion<ax::host_byte_order_tag, _ByteOrderTag> to_target_byte_order;            
        public:
            static const _Ctype* bom () {
                static const _Ctype bom_ [] = {
                    to_target_byte_order::cast (_Ctype (0xFEFF)), 
                    _Ctype (0)};
                return bom_;
            }
            static std::size_t emit (_Ctype* buff_, const char32_t& char_) {
                if (char_ >=  (1ull << _Bits))
                    throw ax::bad_code_point (char_);                
                buff_ [0] = to_target_byte_order::cast (_Ctype (char_));
                return 1u;
            }
            static std::size_t sink (const _Ctype* buff_, std::size_t size_, char32_t& char_) {
                if (size_ < 1)
                    return 0u;
                char_ = buff_ [0];
                return 1u;
            }
        };


        template <std::size_t _Bits, typename _Ctype,
            typename _ByteOrderTag = ax::host_byte_order_tag>
        struct utf;

        template <typename _Ctype, typename _ByteOrderTag>
        struct utf<32, _Ctype, _ByteOrderTag>:
            public ucs<32, _Ctype, _ByteOrderTag>
        {};

        template <typename _Ctype, typename _ByteOrderTag>
        struct utf<16, _Ctype, _ByteOrderTag> {
            typedef _Ctype char_type;
            static const std::size_t max_chars = 2;
            static const std::size_t bom_size = 1;
        protected:
            typedef ax::conversion<ax::host_byte_order_tag, _ByteOrderTag> to_target_byte_order;
            static_assert (sizeof (_Ctype) == 2, "FAILED ASSERTION: sizeof (_Ctype) == 2!");
        public:
            static const _Ctype* bom () {
                static const _Ctype bom_ [] = {
                    to_target_byte_order::cast (_Ctype (0xFEFF)),
                    _Ctype (0)};
                return bom_;
            }

            static std::size_t emit (_Ctype* buff_, const char32_t& char_) {
                if (char_ >= 0xD800 && char_ < 0xE000)
                    throw ax::exception::bad_code_point (char_);
                if (char_ < 0x10000)                    
                    return buff_ [0] = to_target_byte_order::cast (_Ctype (char_)), 1u;                
                auto c = char_ - 0x10000;                
                buff_ [0] = to_target_byte_order::cast (_Ctype (0xD800 + ((c >> 10) & 0x3FF)));
                buff_ [1] = to_target_byte_order::cast (_Ctype (0xDC00 + (c & 0x3FF)));
                return 2u;
            }

            static std::size_t sink (const _Ctype* buff_, std::size_t size_, char32_t& char_) {
                if (size_ < 1)
                    return 0u;
                if (buff_ [0] < 0xD800 || buff_ [0] >= 0xE000) {                   
                    char_ = buff_ [0];
                    return 1u;
                }
                if (buff_ [0] < 0xDC00) {
                    if (size_ < 2)
                        return 0u;
                    char_ = buff_ [0] & 0x3ff;
                    if (buff_ [1] >= 0xDC00) {
                        char_ = (char_ << 10) + (buff_ [1] & 0x3ff) + 0x10000;
                        return 2u;
                    }
                    throw bad_code_point (buff_ [1]);
                }
                throw bad_code_point (buff_ [0]);
            }
        };


        template <typename _Ctype, typename _ByteOrderTag>        
        struct utf<8, _Ctype, _ByteOrderTag> {
            typedef _Ctype char_type;
            static const std::size_t max_chars = 6;
            static const std::size_t bom_size = 3;
        protected:
            typedef ax::conversion<ax::host_byte_order_tag, _ByteOrderTag> to_target_byte_order;
            static_assert (sizeof (_Ctype) == 1, "FAILED ASSERTION: sizeof (_Ctype) == 1!");
        public:
            static const _Ctype* bom () {
                static const _Ctype bom_ [] = {
                    _Ctype (0xEF), _Ctype (0xBB), 
                    _Ctype (0xBF), _Ctype (0)};
                return bom_;
            }
            
            static std::size_t emit (_Ctype* buff_, const char32_t& char_) {                                
                auto i = 0u; 
                switch (bit_scan_reverse (char_)) {
                    case 31:
                        throw ax::exception::bad_code_point (char_);
                    case 30: case 29: case 28: case 27: case 26:
                        buff_ [i++] = _Ctype (0x80 | ((char_ >> 30) & 0x3F));
                    case 25: case 24: case 23: case 22: case 21:
                        buff_ [i++] = _Ctype (0x80 | ((char_ >> 24) & 0x3F));
                    case 20: case 19: case 18: case 17: case 16:
                        buff_ [i++] = _Ctype (0x80 | ((char_ >> 18) & 0x3F));
                    case 15: case 14: case 13: case 12: case 11:
                        buff_ [i++] = _Ctype (0x80 | ((char_ >> 12) & 0x3F));
                    case 10: case 9: case 8: case 7:
                        buff_ [i++] = _Ctype (0x80 | ((char_ >>  6) & 0x3F));
                        buff_ [i++] = _Ctype (0x80 | ((char_ >>  0) & 0x3F));
                        buff_ [0] = _Ctype ((buff_ [0] & (0x7F >> i)) | (0xFF << (8-i)));
                        return i;
                    case 0: case 1: case 2: case 3: case 4: case 5: case 6:
                        buff_ [0] = _Ctype (char_);
                        return 1u;
                }                
                return 0u;
            }

            static std::size_t sink (const _Ctype* pbuff_, std::size_t size_, char32_t& char_) {                
                auto buff_ = reinterpret_cast<const std::uint8_t*> (pbuff_);
                if (size_ < 0x01) return 0u;
                if (buff_ [0] < 0x80) return char_ = buff_ [0], 1u;
                if (buff_ [0] >= 0xFE) throw ax::bad_code_point (buff_ [0]);                
                auto j = count_leading_zeros (std::uint8_t (~buff_ [0]));                
                if (size_  < j) return 0u;
                char_ = buff_ [0] & (0x7Fu >> j);
                for (auto i = 1u; i < j; ++i) {
                    if ((buff_ [i] >> 6) != 0x02)
                        throw ax::bad_code_point (buff_ [i]);
                    char_ = (char_ << 6) + (buff_ [i] & 0x3F);
                }
                return j;

            }
        };
    }

    template <typename _Ctype, typename _ByteOrderType>
    using utf = ax::detail::utf<sizeof (_Ctype) << 3, _Ctype, _ByteOrderType>;

    template <typename _Ctype, typename _ByteOrderType>
    using ucs = ax::detail::ucs<sizeof (_Ctype) << 3, _Ctype, _ByteOrderType>;

    typedef ax::utf<char, ax::host_byte_order_tag> utf8_t;
            
    typedef ax::utf<wchar_t, ax::little_endian_tag> utfwle_t;
    typedef ax::utf<wchar_t, ax::big_endian_tag> utfwbe_t;
    typedef ax::utf<wchar_t, ax::host_byte_order_tag> utfw_t;
            
    typedef ax::utf<char16_t, ax::little_endian_tag> utf16le_t;
    typedef ax::utf<char16_t, ax::big_endian_tag> utf16be_t;
    typedef ax::utf<char16_t, ax::host_byte_order_tag> utf16_t;
            
    typedef ax::utf<char32_t, ax::little_endian_tag> utf32le_t;
    typedef ax::utf<char32_t, ax::big_endian_tag> utf32be_t;
    typedef ax::utf<char32_t, ax::host_byte_order_tag> utf32_t;


    typedef ax::ucs<char, ax::host_byte_order_tag>        ucs1_t;

    typedef ax::ucs<wchar_t, ax::little_endian_tag>       ucswle_t;
    typedef ax::ucs<wchar_t, ax::big_endian_tag>          ucswbe_t;
    typedef ax::ucs<wchar_t, ax::host_byte_order_tag>     ucsw_t;

    typedef ax::ucs<char16_t, ax::little_endian_tag>      ucs2le_t;
    typedef ax::ucs<char16_t, ax::big_endian_tag>         ucs2be_t;
    typedef ax::ucs<char16_t, ax::host_byte_order_tag>    ucs2_t;

    typedef ax::ucs<char32_t, ax::little_endian_tag>      ucs4le_t;
    typedef ax::ucs<char32_t, ax::big_endian_tag>         ucs4be_t;
    typedef ax::ucs<char32_t, ax::host_byte_order_tag>    ucs4_t;

    template <typename _Dtype, typename _Stype>
    inline typename std::enable_if<!std::is_same<_Dtype, _Stype>::value, 
        std::basic_string<typename _Dtype::char_type>>::type
    transcode (std::basic_string<typename _Dtype::char_type>& output_, 
        const std::basic_string<typename _Stype::char_type>& input_) 
    {
        auto idata_ = std::data (input_);
        auto dsize_ = std::size (input_);
        auto icurr_ = std::size_t (), rsize_ = std::size_t ();
        auto ichar_ = char32_t ();
       
        typename _Dtype::char_type obuff_ [_Dtype::max_chars];

        while (rsize_ = _Stype::sink (
            idata_ + icurr_, 
            dsize_ - icurr_, 
            ichar_)) 
        {
            icurr_ += rsize_;
            auto esize_ = _Dtype::emit (obuff_, ichar_);
            output_.append (obuff_, esize_);
        } 
        
        return output_;
    }

    template <typename _Dtype, typename _Stype>
    inline typename std::enable_if<std::is_same<_Dtype, _Stype>::value,
        std::basic_string<typename _Dtype::char_type>>::type
    transcode (std::basic_string<typename _Dtype::char_type>& output_,
        const std::basic_string<typename _Stype::char_type>& input_) 
    {      
        return output_ = input_;
    }

    template <typename _Dtype, typename _Stype>
    auto transcode (const std::basic_string<typename _Stype::char_type>& input_) {
        std::basic_string<typename _Dtype::char_type> output_;
        return transcode<_Dtype, _Stype> (output_, input_);
    }

    namespace literals {
        namespace detail {
            template <typename _Vtype = char>
            struct iconv_literal {
                template <typename _Ctype>
                operator std::basic_string<_Ctype> () const {                    
                    typedef ax::utf<_Vtype, ax::host_byte_order_tag> _Stype;
                    typedef ax::utf<_Ctype, ax::host_byte_order_tag> _Dtype;
                    return ax::transcode<_Dtype, _Stype> (
                        std::basic_string<_Vtype> (data_, size_));
                }

                iconv_literal (const _Vtype* data_, std::size_t size_):
                    data_ (data_),
                    size_ (size_) 
                {}
            private:
                const _Vtype* data_;
                std::size_t size_;
            };
        }

        inline namespace literals {
            auto operator "" _ (const char* data_, std::size_t size_) {
                return detail::iconv_literal<char> (data_, size_);
            }
            auto operator "" _ (const wchar_t* data_, std::size_t size_) {
                return detail::iconv_literal<wchar_t> (data_, size_);
            }
            auto operator "" _ (const char16_t* data_, std::size_t size_) {
                return detail::iconv_literal<char16_t> (data_, size_);
            }
            auto operator "" _ (const char32_t* data_, std::size_t size_) {
                return detail::iconv_literal<char32_t> (data_, size_);
            }
        }

    } 
}

#endif
