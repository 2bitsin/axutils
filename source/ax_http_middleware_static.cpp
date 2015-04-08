#include "ax_http_middleware_static.hpp"
#include "ax_mime.hpp"
#include <fstream>

ax::http::middleware_static::middleware_static (
    path_type const &path_, 
    std::string const &mount_)
:   path_ (path_),
    mount_ (mount_)
{
}
#include <iostream>
bool ax::http::middleware_static::operator()(request const &req_, response &resp_) {
    using namespace std::tr2::sys;    
    if (req_.path ().size () < mount_.size () ||
        mount_ != req_.path ().substr (0, mount_.size ()))
        return false;
    auto rpath_ = path_;
    rpath_.append (req_.path ().substr (mount_.size ()));
    if (is_directory (rpath_))
        rpath_.append ("index.html");
    auto rfile_ = std::ifstream  (rpath_, std::ios::binary);    
    if (!rfile_) {
        return false;
    }
    auto mimet_ = mime (rpath_.extension ().string ());
    resp_.header ("Content-Type", mimet_);
    resp_.header ("Content-Length", std::to_string (file_size (rpath_)));
    resp_.header ("Content-Disposition", "inline; filename=\""+rpath_.filename ().string ()+"\"");
    resp_.header ("Access-Control-Allow-Origin", "*");
    while (rfile_) {
        char buffer_ [2048];
        rfile_.read (buffer_, sizeof (buffer_));
        resp_.send_raw (buffer_, rfile_.gcount ());
    }
    #if defined (__AX_DEBUG__)
    std::cout << "STATIC: " + std::string (req_) + "\n";
    #endif
    return true;
}

std::string ax::http::middleware_static::mime (std::string const &ext_) {
    return ax::util::mime_type::get (ext_);
}

