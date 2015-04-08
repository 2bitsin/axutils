#ifndef __AX_HTTP_REQUEST_HPP__
#define __AX_HTTP_REQUEST_HPP__

#include <unordered_map>
#include <memory>
#include <cstdint>

#include "ax_socket_stream_adapter.hpp"
#include "ax_utils.hpp"

namespace ax {
    namespace http {
        struct request {
            
            template <typename _Sock, typename _USock = 
                typename std::remove_reference<_Sock>::type>
            request (_Sock &&sock_):
                stream_ (std::make_unique<ax::util::socket_stream_adapter<char, _USock>> (
                    std::forward<_Sock> (sock_)))
            {
                std::string line_;
                auto &input_ = *stream_;
                if (!ax::util::getline (input_, line_) || line_.empty ()) 
                    throw std::runtime_error ("Malformed request");
                std::tie (method_, path_, proto_) =
                    ax::util::split_tuple<std::tuple<std::string, std::string, std::string>> (line_);
                if (method_.empty () || path_.empty () || proto_.empty ())
                    throw std::runtime_error ("Malformed request");
                std::tie (path_, query_string_) = 
                    ax::util::split_tuple<std::tuple<std::string, std::string>> (path_, std::string ("?"));
                std::vector<std::string> temp_;
                ax::util::split (query_string_, std::string ("&"), temp_);
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

            inline auto const &path     () const { return path_; };
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
                return *stream_;
            }
            auto &&stream () const {
                return *stream_;
            }

        private:
            std::string method_, path_, proto_, query_string_;
            
            std::unique_ptr<util::socket_stream_adapter_base<char>> stream_;
            std::unordered_map<std::string, std::string> headers_;
            std::unordered_map<std::string, std::string> query_;            
        };

            
    }
}

#endif
