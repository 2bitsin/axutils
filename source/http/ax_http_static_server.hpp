#ifndef __AX_HTTP_MIDDLEWARE_STATIC_HPP__
#define __AX_HTTP_MIDDLEWARE_STATIC_HPP__

#include <filesystem>
#include "ax_http_request.hpp"
#include "ax_http_response.hpp"

namespace ax {
    namespace http {
        struct static_server {
            typedef std::tr2::sys::path path_type;
            static_server (path_type const &public_, std::string const &mount_ = "/");
            bool operator () (request const &req_, response &resp_);            
        private:
            path_type public_;
            std::string mount_;
        };
    }
}


#endif
