#include <iostream>
#include <exception>
#include <thread>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>
#include <tuple>
#include <ctime>
#include <cstdio>

#include "etc/ax_utils.hpp"
#include "net/ax_address.hpp"
#include "net/ax_tcp_socket.hpp"
#include "http/ax_http_listener.hpp"
#include "http/ax_http_static_server.hpp"
#include "http/ax_mime.hpp"


int main (int, char **) try {	    
    std::srand (std::time (nullptr)&0xFFFFFFFF);
    ax::http::listener<ax::tcp::socket> listener_ (
        ax::address::parse ("0.0.0.0:80"));    
    listener_.attach (ax::http::static_server {"./", "/"});
    listener_.attach ([] (ax::http::request &req_, ax::http::response &resp_) {
        auto &&stream_ = req_.stream ();
        return false;
    });

    listener_.join ();

    return 0;
}
catch (std::exception const &e) {
    std::cout << e.what () << "\n";	
    std::cin.get () ;
}
