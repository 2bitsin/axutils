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



    auto rfile_ = ([] (auto const &rpath_, auto &resp_) {
        auto gzpath_ = path (rpath_.string () + ".gz");
        if (exists (gzpath_)) {        
            resp_.header ("Content-Encoding", "gzip");
            return std::ifstream (gzpath_, std::ios::binary);
        }
        return std::ifstream (rpath_, std::ios::binary);
    }) (rpath_, resp_);
    
    if (!rfile_) {
        return false;
    }

    rfile_.seekg (0, std::ios::end);
    auto mtype_ = mime (rpath_.extension ().string ());
    auto fsize_ = rfile_.tellg ();
    rfile_.seekg (0, std::ios::beg);

    resp_.header ("Content-Type", mtype_);
    resp_.header ("Content-Length", fsize_);
    resp_.header ("Content-Disposition", "inline; filename=\""+rpath_.filename ().string ()+"\"");
    resp_.header ("Access-Control-Allow-Origin", "*");
    auto buffsz_ = 1024*1024;
    auto buffer_ = std::make_unique<char []>(buffsz_);
    while (rfile_) {        
        rfile_.read (buffer_.get (), buffsz_);
        resp_.send_raw (buffer_.get (), rfile_.gcount ());
    }
    #if defined (__AX_DEBUG__)
    std::cout << "STATIC: " + std::string (req_) + "\n";
    #endif
    return true;
}

std::string ax::http::middleware_static::mime (std::string const &ext_) {
    return ax::util::mime_type::get (ext_);
}

