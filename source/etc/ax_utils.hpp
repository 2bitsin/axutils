#ifndef __AX_UTILS_HPP__
#define __AX_UTILS_HPP__

#include <memory>
#include <cctype>
#include <type_traits>
#include <array>

namespace ax {
    namespace util {
        template <typename Type, typename U = typename std::remove_reference<Type>::type>
        auto move_unique (Type &&in) {
            return std::make_unique<U> (std::forward<Type> (in));
        }

        template <typename Type, typename U = typename std::remove_reference<Type>::type>
        auto move_shared (Type &&in) {
            return std::make_shared<U> (std::forward<Type> (in));
        }

        namespace detail {
            template <typename _String, typename _Ctype, std::size_t _Count>
            _String __cast_char_array (const _Ctype (&array_) [_Count]) {
                static const auto adjust_ = [] (auto &&array_) {
                    return *std::rend (array_) ? 0 : 1;
                };
                return _String (std::begin (array_), std::end (array_) - adjust_ (array_));
            }
        }

        template <typename _Stream, typename _String, typename _Ctype = _String::value_type>
        inline bool getline (_Stream &stream_, _String &line_, const _String &delim_ = 
            detail::__cast_char_array<_String> ("\r\n")) 
        {
            std::array<_Ctype, 2048> temp_;
            line_ = _String ();
            auto avail_ = decltype (stream_.peek (temp_.data (), temp_.size ())) {};
            while ((avail_ = stream_.peek (temp_.data (), temp_.size ())) > 0) {  
                auto sbuff_ = _String (temp_.data (), avail_);
                auto stpos_ = sbuff_.find (delim_);
                if (stpos_ != sbuff_.npos) {
                    auto trunc_ = stpos_ + delim_.size ();
                    line_.append (temp_.data (), stpos_);
                    stream_.ignore (trunc_);
                    return true;
                }
                stream_.ignore (avail_);
                line_.append (temp_.data (), avail_);                
            }
            return false;
        }

        template <typename _String, typename VectorType>
        auto split (_String const &str_, _String const &needle_, VectorType &vec_) {
            auto i = vec_.size ();
            auto o = std::size_t ();
            do {                
                auto lo = str_.find (needle_, o);
                if (lo != str_.npos) {                
                    vec_.emplace_back (std::begin (str_)+o, std::begin (str_)+lo);
                    o = lo + needle_.size ();
                    continue;
                }
                vec_.emplace_back (std::begin (str_)+o, std::end (str_));
                break;
            }
            while (!0);
            return vec_.size () - i;
        }


        namespace detail {

            template <typename _String, typename _Arg0>
            auto &&__tuple_split (std::tuple<_Arg0> &tuple_, 
                const _String &string_, const _String &delim_)
            {
                std::get<0> (tuple_) = string_;
                return tuple_;
            }

            template <typename _String, typename _Arg0, typename... _Args>
            auto &&__tuple_split (std::tuple<_Arg0, _Args...> &tuple_, 
                const _String &string_, const _String &delim_) 
            {
                auto depos_ = string_.find (delim_);
                if (depos_ != _String::npos) {
                    std::get<0> (tuple_) = string_.substr (0, depos_);                               
                    __tuple_split (reinterpret_cast<std::tuple<_Args...> &> (tuple_), 
                        string_.substr (depos_ + delim_.size ()), delim_);
                    return tuple_;
                }
                std::get<0> (tuple_) = string_;
                return tuple_;
            }

        }

        template <typename _Tuple, typename _String>
            _Tuple split_tuple (_String const &string_, _String const &delim_
                = detail::__cast_char_array<_String> (" ")) 
            {
                _Tuple tmp_;                
                return detail::__tuple_split (tmp_, string_, delim_);
            }
        

        template <typename _String>
        auto trim (_String const &str_) {
            static const auto _is_space = [] (auto c) { 
                static const char space_ [] = "\t\n\f\r ";
                return std::binary_search (std::begin (space_), std::end (space_), c);
            };
            return _String (
                std::find_if_not (std::begin (str_), 
                    std::end (str_), _is_space),
                std::find_if_not (std::rbegin (str_), 
                    std::rend (str_), _is_space).base ()
            );
                
        }
    }
}

#endif
