#include "ax_http_response.hpp"


//template <typename _Ctype>

ax::http::response &
    ax::http::response::send (std::tr2::sys::path const & public_) 
{
    std::ifstream ifs_ (public_, std::ios::binary);
    _default_header ("Content-Length", std::tr2::sys::file_size (public_));
    _default_header ("Content-Disposition",
        "inline; filename=\""+public_.filename ().string ()+"\"");
    _default_header ("Content-Type",
        mime_type::get (public_.extension ().string ()).string ());
    return send (ifs_);
}

bool ax::http::response::_has_header (std::string const & key_) {
    auto i = headers_.find (key_);
    return i != headers_.end ();
}

bool ax::http::response::_send_headers () {
    if (headers_sent_)
        return false;
    stream_.put (http_head_ + "\r\n");
    for (auto &&header_ : headers_) {
        stream_.put (header_.first + ": " +
            header_.second + "\r\n");
    }
    stream_.put ("\r\n");
    return headers_sent_ = true;
}
