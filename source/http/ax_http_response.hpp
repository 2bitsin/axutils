#ifndef __AX_HTTP_RESPONSE_HPP__
#define __AX_HTTP_RESPONSE_HPP__

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <array>
#include <filesystem>
#include <unordered_map>
#include <iostream>
#include "../etc/ax_utils.hpp"
#include "../net/ax_socket_stream_adapter.hpp"
#include "ax_mime.hpp"

namespace ax {
    namespace http {
        
        struct response {

            response (ax::util::socket_stream_adapter_base<char> &pstream_)
            :   stream_       (pstream_),
                http_head_    ("HTTP/1.1 200 OK"),
                headers_      ({}),
                headers_sent_ (false)
            {
            }

            inline response &header (std::string const &value_) {
                if (headers_sent_)
                    throw std::logic_error ("Unabled to set headers after they are sent");
                http_head_ = value_;
                return *this;
            }

            template <typename _Value>
            inline response &header (std::string const &key_, _Value &&value_) {
                if (headers_sent_)
                    throw std::logic_error ("Unabled to set headers after they are sent");
                headers_ [key_] = util::to_string<char> (std::forward<_Value> (value_));
                return *this;
            }

            auto &&stream () {
                !headers_sent_ && _send_headers ();
                return stream_;
            }

            template <typename _Ctype>
            inline response &send (std::basic_string<_Ctype> const &data_) {  
                _default_header ("Content-Type", mime_type::get ("txt").string ());
                return _send (data_.c_str (), data_.length ());
            }

            template <typename _Vtype>
            inline response &send (std::vector<_Vtype> const &data_) {
                return _send (data_.data (), data_.size ());
            }

            template <typename _Vtype>
            inline response &send (std::initializer_list<_Vtype> const &data_) {
                return _send (data_.begin (), data_.size ());
            }

            template <typename _Vtype, std::size_t _Count>
            inline response &send (std::array<_Vtype, _Count> const &data_) {
                return _send (data_.data (), data_.size ());
            }

            template <typename _Vtype, std::size_t _Count>
            inline response &send (const _Vtype (&data_) [_Count]) {
                return _send (std::begin (data_), _Count);
            }

            template <typename _Ctype>
            inline response &send (std::basic_ifstream<_Ctype> &file_) {
                file_.seekg (0, std::ios::end);
                auto filesz_ = file_.tellg ();
                file_.seekg (0, std::ios::beg);
                _default_header ("Content-Length", filesz_ * sizeof (_Ctype));
                auto length_ = 1024*16 / sizeof (_Ctype);
                auto buffer_ = std::make_unique<_Ctype []> (length_);
                for (;;) {
                    file_.read (buffer_.get (), length_);
                    if (!file_.gcount ()) 
                        break;
                    _send (buffer_.get (), file_.gcount ());
                    if (!file_ || file_.gcount () != length_)
                        break;
                }
                return *this;
            }

            //template <typename _Ctype>
            response &send (std::tr2::sys::path const &public_);

        private:
            bool _has_header (std::string const &key_);
            bool _send_headers ();

            template <typename _Value>
            response &_default_header (std::string const &key_, _Value &&val_) {
                headers_.insert (std::make_pair (key_, 
                    util::to_string<char> (std::forward<_Value> (val_))));
                return *this;
            }

            template <typename _Ctype, typename _Size>
            response &_send (_Ctype const *data_, _Size size_) {
                auto length_ = size_ * sizeof (_Ctype);
                if (!headers_sent_) {
                    _default_header ("Content-Length", length_);
                    _default_header ("Content-Type", mime_type::get ("bin").string ());
                }
                auto len_ = std::intptr_t (size_ * sizeof (_Ctype));
                auto ptr_ = reinterpret_cast<char const *> (data_);
                if (stream ().write (ptr_, len_) != len_) {
                    throw std::runtime_error ("Unable to send response");
                }
                return *this;
            }

        private:
            util::socket_stream_adapter_base<char> &stream_;
            std::string http_head_;
            std::unordered_map<std::string, std::string> headers_;
            bool headers_sent_;
        };

    }
}

#endif

