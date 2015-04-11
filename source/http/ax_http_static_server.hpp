#ifndef __AX_HTTP_MIDDLEWARE_STATIC_HPP__
#define __AX_HTTP_MIDDLEWARE_STATIC_HPP__

#include <filesystem>
#include "ax_http_request.hpp"
#include "ax_http_response.hpp"

namespace ax {
    namespace http {
        struct static_server {
            enum flags {
                directory_index = 1
            };
            typedef std::tr2::sys::path path_type;
            static_server (path_type const &public_, std::string const &mount_ = "/", std::uint32_t flags_ = directory_index);
            bool operator () (request const &req_, response &resp_);
            
            bool has_directory_index () const;
            bool directory_listing (path_type const &path_, request const &req_, response &resp_);
        private:
            path_type public_;
            std::string mount_;
            std::uint32_t flags_;
        };
    }
}


#endif
