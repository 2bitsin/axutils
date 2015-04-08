#include "ax_http_response.hpp"
namespace ax{
    namespace http{
        template<typename _String>
            typename basic_response<_String>::string const &
                basic_response<_String>::header (string const &key) const {
                return headers_.at (key);
            }

        //template <typename _String>
        //    basic_response<_String> &
        //        basic_response<_String>::header (string const &key, string const &value) 
        //    {
        //        headers_ [key] = value;
        //        return *this;
        //    }

        template <typename _String>
            typename basic_response<_String>::string const &
                basic_response<_String>::header () const 
            {
                return response_line_;
            }

        template <typename _String>
            basic_response<_String> &
                basic_response<_String>::header (string const &value) 
            {
                response_line_ = value;
                return *this;
            }

        template <typename _String>
            basic_response<_String> &basic_response<_String>::send_headers () {                
                if (headers_sent_) {                    
                    return *this;
                }

                if (response_line_.size () < 1){
                    throw std::runtime_error ("Response line empty");
                }
                    
                auto terminate_ = string () + char_type ('\r') + char_type ('\n');
                auto separator_ = string () + char_type (':') + char_type (' ');


                send_string (response_line_ + terminate_);
                for (auto const &header_: headers_) {
                    send_string ( 
                        header_.first + separator_ + 
                        header_.second + terminate_);
                }
                send_string (terminate_);                
                headers_sent_ = true;
                return *this;
            }

        template<typename _String>
            basic_response<_String> &basic_response<_String>::send_raw (void const *data_, std::intptr_t const &len_) {
                send_headers ();
                if (!chunked ()) {
                    if (output_ (data_, len_) < len_)
                        throw std::runtime_error ("Unabe to send all data");
                    return *this;
                }
                //TODO: implement chunked sending
                throw std::runtime_error ("Chunked encoding not implemented");
                return *this;
            }

        template<typename _String>
            bool basic_response<_String>::chunked () const {
                return chunked_;
            }

        template <typename _String>
            basic_response<_String> &
                basic_response<_String>::chunked (bool ck) 
            {
                chunked_ = ck;
                return *this;
            }

        template <typename _String>
            basic_response<_String> &
                basic_response<_String>::send_string (string const &str_) 
            {
                auto length_ = std::intptr_t (str_.size () * sizeof (str_ [0]));
                if (output_ (str_.c_str (), length_) < length_)
                    throw std::runtime_error ("Unable to complete send");
                return *this;
            }

    }
}

template struct ax::http::basic_response<std::string>;