#ifndef __AX_STRING_RESPONSE_HPP__
#define __AX_STRING_RESPONSE_HPP__

#include "ax_http_response.hpp"

namespace ax{
    namespace http {
        
        template <typename _String = std::string>
            struct string_response {
                typedef _String value_type;
                template <typename Type> friend struct basic_response;
                string_response (value_type const &string_);
            private:
                template <typename Response>
                    string_response const &send (Response &resp_) const;                
                value_type value_;
            };

        template<typename _String>
            inline string_response<_String>::string_response (
                value_type const &string_)
            :   value_ (string_)
            {}

        template <typename _String>
        template <typename Response>
            inline string_response<_String> const &
                string_response<_String>::send (Response &resp_) const
            {                
                auto length_ = value_.size () * sizeof (value_ [0]);
                resp_.header ("Content-Length", std::to_string (length_));
                resp_.send_raw (&value_ [0], length_);                
                return *this;
            }

        template <typename Type>
            string_response<std::basic_string<Type>> 
                make_string_response (std::basic_string<Type> const &str_) 
            { 
                return {str_};
            }

        template <typename Type, std::size_t Size>
            string_response<std::basic_string<Type>> 
                make_string_response (Type const (&str_) [Size])
            { 
                return {str_};
            }
        

    }
}


#endif