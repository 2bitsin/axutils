#ifndef __AX_HTTP_RESPONSE_HPP__
#define __AX_HTTP_RESPONSE_HPP__

#include <string>
#include <functional>
#include <unordered_map>

namespace ax {
    namespace http {
        template <typename _String = std::wstring>
            struct basic_response {
                typedef _String string;
                typedef typename _String::value_type char_type;

                string const &header (string const &key) const;
                template <typename _Value>
                basic_response &header (string const &key, _Value const &value);
                string const &header () const;
                basic_response &header (string const &);

                template <typename OutputFascet>
                basic_response (OutputFascet &&);

                template <typename Type>
                basic_response &send (Type const &what);
                                
                basic_response &send_headers ();
                basic_response &send_raw (void const *_data, std::intptr_t const &size_);
                
                bool chunked () const;
                basic_response &chunked (bool ck);
                
            private:
                basic_response &send_string (string const &);

                bool headers_sent_ = false;
                bool chunked_ = false;
                string response_line_ = string ();                
                std::function<std::intptr_t (void const *buffer, std::intptr_t size)> output_;
                std::unordered_map<string, string> headers_;
            };


            namespace basic_response_detail{
                template <typename _String, typename _Value>
                    _String make_string (_Value const &value) {
                        auto val_ = std::to_string (value);
                        return _String (std::begin (val_), std::end (val_));
                    }
                template <typename _String, typename _Value, std::size_t _Count>
                    _String make_string (_Value const (&value) [_Count]) {
                        return _String (value);
                    }
                template <typename _String, typename _Value>                    
                    _String make_string (std::basic_string<_Value> const &value) {
                        return _String (std::begin (value), std::end (value));
                    }
            }

            template <typename _String>
            template <typename _Value>
            basic_response<_String> &basic_response<_String>::header (string const &key, _Value const &value) 
            {
                headers_ [key] = basic_response_detail::make_string<_String> (value);
                return *this;
            }

            template <typename _String>
        template <typename OutputFascet>
            inline basic_response<_String>::basic_response (OutputFascet &&out_):
                output_ (out_)                
            {
                static const char ok_ [] = "HTTP/1.1 200 Ok";
                static const char ctk_ [] = "Content-Type";
                static const char ctv_ [] = "text/html";
                header (string (std::begin (ok_), std::end (ok_)-1));
                header (string (std::begin (ctk_), std::end (ctk_)-1),
                    string (std::begin (ctv_), std::end (ctv_)-1));
            }

        template<typename _String>
        template <typename Type>
            inline basic_response<_String> &
                basic_response<_String>::send (Type const &what_) 
            {
                what_.send (*this);
                return *this;
            }

        typedef basic_response<std::string> response;

    }
}

#endif

