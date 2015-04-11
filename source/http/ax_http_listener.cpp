#include "../net/ax_tcp_socket.hpp"
#include "ax_http_listener.hpp"
#include "ax_http_assets.hpp"

namespace ax {
    namespace http{

    template <typename _Stream>
        inline listener<_Stream>::listener (
            address_type const &addr_, int backlog_)         
        :   sock_ (socket_type::listen (addr_, backlog_)),
            active_ (0)            
        {}

    template <typename _Stream>
        inline listener<_Stream> &
            listener<_Stream>::join () 
        {
            
            for (;;) {
                std::thread ([&, rqso_ = ax::util::move_unique (sock_.accept ())] () {
                    ++active_;
                    try {                       
                        this->client (*rqso_);
                    }
                    catch (std::exception const &e){  
                        this->log (e.what ());
                    }
                    --active_;
                }).detach ();
            }
            return *this;
        }

    template <typename _Stream>
        inline listener<_Stream> &
            listener<_Stream>::log (std::string const &e) 
        {            
            std::cout << e << "\n";
            return *this;
        }

    template <typename _Stream>
        inline listener<_Stream> &
            listener<_Stream>::client (socket_type &client_) 
        {                      
            auto stream_   = util::socket_stream_adapter<char, socket_type> (client_);
            auto request_  = request  (stream_);        
            auto response_ = response (stream_);
            try {
                for (auto &&mw_: middleware_)
                    if (mw_ (request_, response_))
                        return *this;                
                assets::send_404 (request_, response_);
                return *this;
            }
            catch (std::exception const &e){ 
               assets::send_500 (request_, response_, e);
            }
            return *this;
        }

    }
    
}

template struct ax::http::listener<ax::tcp::socket>;
