#ifndef __AX_HTTP_REQUEST_HPP__
#define __AX_HTTP_REQUEST_HPP__

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "../net/ax_socket_stream_adapter.hpp"
#include "../etc/ax_utils.hpp"

namespace ax {
    namespace http {
        struct request {
            
            request (ax::util::socket_stream_adapter_base<char> &pstream_)
            :   stream_ (pstream_)
            {
                std::string line_;
                auto &input_ = stream_;
                if (!ax::util::getline (input_, line_) || line_.empty ()) 
                    throw std::runtime_error ("Malformed request");
                std::tie (method_, public_, proto_) =
                    ax::util::split_tuple<std::tuple<std::string, std::string, std::string>> (line_);
                if (method_.empty () || public_.empty () || proto_.empty ())
                    throw std::runtime_error ("Malformed request");
                std::tie (public_, query_string_) = 
                    ax::util::split_tuple<std::tuple<std::string, std::string>> (public_, std::string ("?"));
                
                auto temp_ = ax::util::split<std::vector> (
                    query_string_, std::string ("&"));
                for (auto &&qk_: temp_) {
                    std::string key_, value_;
                    std::tie (key_, value_) = ax::util::split_tuple<std::tuple<
                        std::string, std::string>> (qk_, std::string ("="));
                    query_ [key_] = value_;
                }
                while (ax::util::getline (input_, line_) && !line_.empty ()) {                    
                    std::string key_, value_;
                    std::tie (key_, value_) = ax::util::split_tuple<
                        std::tuple<std::string, std::string>> (line_, std::string (":"));
                    headers_ [key_] = ax::util::trim (value_);                                    
                }
            }

            inline auto const &path     () const { return public_; };
            inline auto const &method   () const { return method_; };
            inline auto const &proto    () const { return proto_; };
            inline auto const &header   () const { return headers_ ; }
            inline auto const &query    () const { return query_ ; }
            inline auto const &header   (std::string const &key_) const { return headers_.at (key_) ; }
            inline auto const &query    (std::string const &key_) const { return query_.at (key_) ; }

            operator std::string () const {
                return method () + " " + path () + (query_string_.length () ? "?" + query_string_ : "");
            }

            auto &&stream () {
                return stream_;
            }
            auto &&stream () const {
                return stream_;
            }

        private:
            std::string method_, public_, proto_, query_string_;            
            util::socket_stream_adapter_base<char> &stream_;
            std::unordered_map<std::string, std::string> headers_;
            std::unordered_map<std::string, std::string> query_;            
        };

            
    }
}

#endif
