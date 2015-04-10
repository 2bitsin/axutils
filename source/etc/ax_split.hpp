#ifndef __AX_UTIL_SPLIT_HPP__
#define __AX_UTIL_SPLIT_HPP__

#include <vector>
#include <tuple>
#include <cstddef>

namespace ax{
    namespace util{
        template <typename _String, typename _Collection>
        auto split (_String const &str_, _String const &needle_, _Collection &collection_) {
            auto i = collection_.size ();
            auto o = std::size_t ();
            do {                
                auto lo = str_.find (needle_, o);
                if (lo != str_.npos) {                
                    collection_.push_back ({std::begin (str_)+o, std::begin (str_)+lo});
                    o = lo + needle_.size ();
                    continue;
                }
                collection_.push_back ({std::begin (str_)+o, std::end (str_)});
                break;
            }
            while (!0);
            return collection_.size () - i;
        }

        template <template <typename T, typename=std::allocator<T>> class _Container, typename _String>
        auto split (_String const &str_, _String const &needle_) {
            _Container<_String> container_;
            split (str_, needle_, container_);
            return container_;
        }

        template <template <typename T> class _Container, typename _String>
        auto split (_String const &str_, _String const &needle_) {
            _Container<_String> container_;
            split (str_, needle_, container_);
            return container_;
        }


        namespace detail {

            template <typename _String, typename _Arg0>
            auto &&__tuple_split (std::tuple<_Arg0> &tuple_, 
                const _String &string_, const _String &delim_)
            {
                std::get<0> (tuple_) = from_string<_Arg0> (string_);
                return tuple_;
            }

            template <typename _String, typename _Arg0, typename... _Args>
            auto &&__tuple_split (std::tuple<_Arg0, _Args...> &tuple_, 
                const _String &string_, const _String &delim_) 
            {
                auto depos_ = string_.find (delim_);
                if (depos_ != _String::npos) {
                    std::get<0> (tuple_) = from_string<typename std::tuple_element<0, 
                        std::tuple<_Arg0, _Args...>>::type> (
                            string_.substr (0, depos_));
                    __tuple_split (reinterpret_cast<std::tuple<_Args...> &> (tuple_), 
                        string_.substr (depos_ + delim_.size ()), delim_);
                    return tuple_;
                }
                std::get<0> (tuple_) = from_string<typename std::tuple_element<0, 
                    std::tuple<_Arg0, _Args...>>::type> (string_);
                return tuple_;
            }

        }

        template <typename _Tuple, typename _String>
        _Tuple split_tuple (_String const &string_, _String const &delim_
            = ax::util::to_string<typename _String::value_type> (" ")) 
        {
            _Tuple tmp_;                
            return detail::__tuple_split (tmp_, string_, delim_);
        }

    }
}

#endif
