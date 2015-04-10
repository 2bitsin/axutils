#include "ax_http_static_server.hpp"
#include "ax_mime.hpp"
#include <fstream>

namespace sys = std::tr2::sys;   
    


ax::http::static_server::static_server (
    path_type const &public_, 
    std::string const &mount_)
:   public_ (sys::canonical (public_)),
    mount_ (mount_)
{}


bool ax::http::static_server::operator () (request const &req_, response &resp_) {    
    if (req_.path ().size () < mount_.size () ||
        mount_ != req_.path ().substr (0, mount_.size ()))
        return false;
    auto rpath_ = public_;
    rpath_.append (req_.path ().substr (mount_.size ()));
    if (sys::is_directory (rpath_))
        rpath_.append ("index.html");
    rpath_ = sys::canonical (rpath_);
    auto dirstr_ = sys::canonical (rpath_.parent_path ()).string ();
    if (dirstr_.find (public_.string ()) != dirstr_.npos) {
        auto gzpath_ = sys::path (rpath_.string () + ".gz");
        if (sys::exists (gzpath_)) {
            resp_.header ("Content-Encoding", "gzip");
            resp_.header ("Content-Type", mime_type::get (rpath_.extension ().string ()).string ());
            resp_.header ("Content-Disposition", "inline; filename=\""+rpath_.filename ().string ()+"\"");
            resp_.send (gzpath_);
            return true;
        }
        if (sys::exists (rpath_)) {
            resp_.send (rpath_);
            return true;
        }
        return false;
    }
    return false;
}


