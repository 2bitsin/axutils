#include "ax_tcp_socket.hpp"
#include "ax_http_listener.hpp"

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
            auto request_ = request (client_);
            /*log (request_.method () + " \"" + request_.path () + "\" \"" + 
                request_.header ("User-Agent") + "\" [" + 
                client_.address ().host () + "]");*/
        
            auto response_ = response (
                response::output_fascet_flag, 
                std::bind (
                    &socket_type::send, &client_, 
                    std::placeholders::_1, 
                    std::placeholders::_2, 
                    false));
            //auto response_ = response (response::output_fascet_flag, 
            //    [&client_] (auto &&buff_, auto &&size_) {
            //        return client_->send (buff_, size_);
            //    });
            try {
                for (auto &&mw_: middleware_) {
                    if (!mw_ (request_, response_))
                        continue;
                    return *this;
                }

                response_.header ("HTTP/1.1 404 Page not found");
                response_.header ("Content-Type", "text/html");            
                response_.header ("Content-Type", "text/html");            
                response_.header ("Content-Type", "text/html");            
                response_.send (make_string_response (std::string (
                    "<html><head><title>404 - Page not found</title></head>"
                    "<body><h1>404 - Page not found</h1></body></html>"
                )));
                return *this;
            }
            catch (std::exception const &e){ 
                response_.header ("HTTP/1.1 500 Internal server error");
                response_.header ("Content-Type", "text/html");            
                response_.send (make_string_response (std::string (
                    "<html><head><title>500 - Internal server error</title></head>"
                    "<body><h1>500 - Internal server errror</h1><p>"+std::string (e.what ())+"</p></body></html>"
                )));                
            }
            return *this;
        }

    }
    
}

template struct ax::http::listener<ax::tcp::socket>;
