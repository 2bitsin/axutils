#include "ax_http_static_server.hpp"
#include "ax_http_assets.hpp"
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
    auto dpath_ = rpath_;
    if (sys::is_directory (rpath_)) {
        rpath_.append ("index.html");
    }
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

        return directory_listing (dpath_, req_, resp_);
    }
    return false;
}

bool ax::http::static_server::has_directory_index () const {
    return flags_ & directory_index;
}

bool ax::http::static_server::directory_listing (path_type const &path_, request const &req_, response & resp_) {            
    if (has_directory_index () && 
        sys::exists (path_) && 
        sys::is_directory (path_)) 
    {     
        struct dir_index_entry {

            dir_index_entry (
                std::string const &href_, 
                std::string const &title_, 
                bool directory_, 
                bool up_directory_)
            :   href_ (href_),
                title_ (title_),
                directory_ (directory_),
                up_directory_ (up_directory_)
            {}

            auto const &title () const {return title_;}
            auto const &href () const {return href_;}
            auto const &directory () const {return directory_;}
            auto const &up_directory () const {return up_directory_;}
                
        private:
            std::string href_, title_;
            bool directory_, up_directory_;
        };
               
        auto entries_ = std::vector<dir_index_entry>  {
            {sys::path (req_.path ()).append ("..").string (), "..", true, true}
        };        
        std::for_each (
            sys::directory_iterator (path_),
            sys::directory_iterator (), 
            [&entries_, &req_] (auto &&dir_) {
                bool is_dir_ =  sys::is_directory (dir_.path ());
                auto name_ = dir_.path ().filename ();
                auto next_ = sys::path (req_.path ()).append (name_);
                entries_.emplace_back (next_.string (), name_.string (), is_dir_, false);        
            });
        assets::generate_directory_listing (
            req_, resp_, std::begin (entries_), 
            std::end (entries_));        
        return true;
    }
    return false;
}


