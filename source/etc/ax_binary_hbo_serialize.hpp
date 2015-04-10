#ifndef __AX_HBO_SERIALIZE_HPP__
#define __AX_HBO_SERIALIZE_HPP__

#include <vector>
#include <string>
#include <array>
#include <initializer_list>

namespace ax {
    namespace util{
        namespace hbo {
            ///////////////////////////////////////////////////            
            template <typename _Value>
            struct serialize {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const &val_) {                    
                    return std::forward<_Stream> (os_) (
                        &val_, sizeof (val_));
                }
            };

            template <typename _Value, std::size_t _Count>
            struct serialize<const _Value [_Count]> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const (&val_) [_Count]) {
                    return std::forward<_Stream> (os_) (
                        &val_ [0], sizeof (val_));
                }
            };

            template <typename _Value, std::size_t _Count>
            struct serialize<_Value [_Count]>: 
                serialize<const _Value [_Count]>
            {};

            template <typename _Value>
            struct serialize<_Value const *> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, _Value const *val_) {
                    static auto const count_ = [] (auto const *val_) {
                        auto len_ = std::size_t {};
                        while (val_ [len_] != _Value ()) ++len_;
                        return len_;
                    };
                    return std::forward<_Stream> (os_) (
                        val_, sizeof (_Value) * count_ (val_));
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
            struct serialize<const std::basic_string<_Value>> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::basic_string<_Value> &val_) 
                {
                    return std::forward<_Stream> (os_) (
                        val_.c_str (), sizeof (_Value) * val_.size ());
                }
            };

            template <typename _Value>
            struct serialize<std::basic_string<_Value>>:
                serialize<const std::basic_string<_Value>>
            {};

            template <typename _Value, std::size_t _Count>
            struct serialize<const std::array<_Value, _Count>> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::array<_Value, _Count> &val_) 
                {
                    return std::forward<_Stream> (os_) (
                        val_.data (), sizeof (_Value) * val_.size ());
                }
            };
                
            template <typename _Value, std::size_t _Count>
            struct serialize<std::array<_Value, _Count>>:
                serialize<const std::array<_Value, _Count>>
            {};

            template <typename _Value>
            struct serialize<const std::vector<_Value>> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::vector<_Value> &val_) 
                {
                    return std::forward<_Stream> (os_) (
                        val_.data (), sizeof (_Value) * val_.size ());
                }
            };
                
            template <typename _Value>
            struct serialize<std::vector<_Value>>:
                serialize<const std::vector<_Value>>
            {};

            template <typename _Value>
            struct serialize<const std::initializer_list<_Value>> {
                template <typename _Stream>
                inline static auto &&write (_Stream &&os_, 
                    const std::initializer_list<_Value> &val_) 
                {
                    return std::forward<_Stream> (os_) (
                        val_.begin (), sizeof (_Value) * val_.size ());
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
