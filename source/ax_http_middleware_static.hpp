#ifndef __AX_HTTP_MIDDLEWARE_STATIC_HPP__
#define __AX_HTTP_MIDDLEWARE_STATIC_HPP__

#include <filesystem>
#include "ax_http_request.hpp"
#include "ax_http_response.hpp"

namespace ax {
    namespace http {
        struct middleware_static {
            typedef std::tr2::sys::path path_type;
            middleware_static (path_type const &path_, std::string const &mount_ = "/");
            bool operator () (request const &req_, response &resp_);
            static std::string mime (std::string const &ext_);
        private:
            path_type path_;
            std::string mount_;
        };
    }
}


#endif
