#ifndef __AX_HTTP_LISTENER_HPP__
#define __AX_HTTP_LISTENER_HPP__

#include "../net/ax_tcp_socket.hpp"
#include "../etc/ax_utils.hpp"
#include "ax_http_request.hpp"
#include "ax_http_response.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <thread>
#include <atomic>

namespace ax {
    namespace http{

    template <typename _Stream = ax::tcp::socket>
        struct listener {
            typedef _Stream socket_type;
            typedef typename _Stream::address_type address_type;

            listener (address_type const &addr_, int backlog_ = socket_type::LISTEN_MAX_BACKLOG);
            listener &join ();
            listener &client (socket_type &sock_);
            listener &log (std::string const &e);
            template <typename _Middleware>
                listener &attach (_Middleware &&e);            
        private:                                
            std::vector<std::function<int (request &, response &)>> 
                middleware_;
            socket_type sock_;
            std::atomic<int> active_;
        };


    template <typename _Stream>
    template <typename _Middleware>
        inline listener<_Stream> &
            listener<_Stream>::attach (_Middleware &&mw) 
        {
            middleware_.emplace_back (std::forward<_Middleware> (mw));
            return *this;
        }
    }
    
}


#endif
