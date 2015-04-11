#include "ax_http_static_server.hpp"
#include "ax_mime.hpp"
#include <fstream>

namespace sys = std::tr2::sys;   
    


ax::http::static_server::static_server (
    path_type const &public_, 
    std::string const &mount_,
    std::uint32_t flags_)
:   public_ (sys::canonical (public_)),
    mount_ (mount_),
    flags_ (flags_)
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

        return directory_listing (rpath_, req_, resp_);
    }
    return false;
}

bool ax::http::static_server::has_directory_index () const {
    return flags_ & directory_index;
}

bool ax::http::static_server::directory_listing (path_type const &path_, request const &req_, response & resp_) {    
    auto rpath_ = path_.parent_path ();
    if (sys::is_directory (rpath_) && 
        has_directory_index ()) 
    {
        auto header_ = R"(
            <link rel="stylesheet" type="text/css" href="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css" />
            <script type="application/javascript" src="https://code.jquery.com/jquery-2.1.3.min.js"></script>
            <script type="application/javascript" src="http://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/js/bootstrap.min.js"></script>
        )";
        auto back_ = sys::path (req_.path ());
        back_.append ("..");
        resp_.header ("Content-Type", "text/html");
        std::string index_ = "<html><head><title>"+req_.path ()+
            "</title>"+header_+"</head><body><div class='container'><h1>"+
            req_.path ()+"</h1><table class='table table-striped'>"+
            "<tr><td style='width:30pt'><span class='glyphicon glyphicon-folder-close'>"
            "</span></td><td><a href=\""+back_.string ()+"\">..</a></td></tr>";
        for (auto i = sys::directory_iterator (rpath_); 
            i != sys::directory_iterator (); ++i) 
        {
            bool is_dir_ =  sys::is_directory (i->path ());
            auto name_ = i->path ().filename ().string ();
            auto next_ = sys::path (req_.path ());
            next_.append (name_);
            index_.append ("<tr><td style='width:30pt'><span class='glyphicon glyphicon-"+
                std::string (is_dir_ ? "folder-open" : "file")+
                "'></span></td><td><a href=\""+next_.string ()+"\">"+
                name_+"</a></td></tr>");
        }
        index_.append ("</table></div></body></html>");
        resp_.send (index_);
        return true;
    }
    return false;
}


