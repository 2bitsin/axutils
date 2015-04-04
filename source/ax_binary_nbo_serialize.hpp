#ifndef __AX_BINARY_NBO_SERIALIZE_HPP__
#define __AX_BINARY_NBO_SERIALIZE_HPP__

#include "ax_binary_hbo_serialize.hpp"
#include "ax_byte_swap.hpp"
#include <algorithm>
#include <memory>

namespace ax {
    namespace util{
        namespace nbo {
            ///////////////////////////////////////////////////            
            template <typename _Vtype>
            struct serialize_base {
            protected:
                template <typename _Iter, typename _Stream>
                inline static auto &&_write (_Stream &&os_, _Iter begin_, _Iter end_) {
                    auto len_ = std::distance (begin_, end_);
                    auto tmp_ = std::make_unique<_Vtype []> (len_);
                    std::transform (begin_, end_, tmp_.get (),
                        &host_to_network_byte_order<const _Vtype &>);                    
                    return std::forward<_Stream> (os_) (
                        tmp_.get (), sizeof (_Vtype) * len_);
                }
            };


            template <typename _Value>
            struct serialize: 
                serialize_base<_Value> 
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const &val_) { 
                    auto tmp_ = host_to_network_byte_order (val_);
                    return std::forward<_Stream> (os_) (&tmp_, sizeof (tmp_));
                }
            };

            template <typename _Value, std::size_t _Count>
            struct serialize<const _Value [_Count]>: 
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const (&val_) [_Count]) {
                    return _write (std::forward<_Stream> (os_), std::begin (val_), std::end (val_));   
                }
            };

            template <typename _Value, std::size_t _Count>
            struct serialize<_Value [_Count]>: 
                serialize<const _Value [_Count]>
            {};

            template <typename _Value>
            struct serialize<_Value const *>:
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const *val_) {
                    static auto const count_ = [] (auto const *val_) {
                        auto len_ = std::size_t {};
                        while (val_ [len_] != _Value ()) ++len_;
                        return len_;
                    };
                    return _write (std::forward<_Stream> (os_), val_, val_+count_ (val_));
                }
            };

            template <typename _Value>
            struct serialize<_Value *>: 
                serialize<const _Value *> 
            {};

            template <typename _Value>
            struct serialize<const _Value []>: 
                serialize<const _Value *> 
            {};

            template <typename _Value>
            struct serialize<_Value []>: 
                serialize<const _Value []> 
            {};

        ///////////////////////////////////////////////////            
            template <typename _Value>
            struct serialize<const std::basic_string<_Value>>:
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::basic_string<_Value> &val_) 
                {
                    return _write (std::forward<_Stream> (os_), std::begin (val_), std::end (val_));
                }
            };

            template <typename _Value>
            struct serialize<std::basic_string<_Value>>:
                serialize<const std::basic_string<_Value>>
            {};

            template <typename _Value, std::size_t _Count>
            struct serialize<const std::array<_Value, _Count>>: 
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::array<_Value, _Count> &val_) 
                {
                    return _write (std::forward<_Stream> (os_), std::begin (val_), std::end (val_));
                }
            };
                
            template <typename _Value, std::size_t _Count>
            struct serialize<std::array<_Value, _Count>>:
                serialize<const std::array<_Value, _Count>>
            {};

            template <typename _Value>
            struct serialize<const std::vector<_Value>>:
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::vector<_Value> &val_) 
                {
                    return _write (std::forward<_Stream> (os_), std::begin (val_), std::end (val_));
                }
            };
                
            template <typename _Value>
            struct serialize<std::vector<_Value>>:
                serialize<const std::vector<_Value>>
            {};

            template <typename _Value>
            struct serialize<const std::initializer_list<_Value>>:
                serialize_base<_Value>
            {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::initializer_list<_Value> &val_) 
                {
                    return _write (std::forward<_Stream> (os_), std::begin (val_), std::end (val_));
                }
            };
                
            template <typename _Value>
            struct serialize<std::initializer_list<_Value>>:
                serialize<const std::initializer_list<_Value>>
            {};
        }
    }
}


#endif