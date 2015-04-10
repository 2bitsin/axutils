#ifndef __AX_SOCKET_HPP__
#define __AX_SOCKET_HPP__

#include "ax_address.hpp"
#include <functional>
#include <tuple>
#include <vector>

namespace ax {
    namespace tcp {
        struct socket {
            typedef ax::address address_type;
            
            static const int LISTEN_MAX_BACKLOG;
            static const std::intptr_t NULL_SOCKET;
            static const int ERROR = -1;
            static const int CLOSED = 0;
            static const int SEND = 1;
            static const int RECV = 2;

            static socket connect (address const &);
            static socket listen (address const &, int backlog_ = LISTEN_MAX_BACKLOG);
            socket accept () const;
            
            socket (socket const &) = delete;
            socket (socket &&);
            socket &operator = (socket const &) = delete;
            socket &operator = (socket &&);
            ~socket ();
            ax::address const &address () const;
            std::intptr_t raw_socket () const;
            std::intptr_t detatch ();
            socket &shutdown (int what_);
            socket &close ();

            operator bool () const;

            std::intptr_t recv (void *buffer_, std::intptr_t len_, bool oob_ = false);
            std::intptr_t peek (void *buffer_, std::intptr_t len_, bool oob_ = false);
            std::intptr_t send (void const *buffer_, std::intptr_t len_, bool oob_ = false);

            std::size_t pending () const;
            socket &blocking (bool = false);

            socket ();

            socket &timeout (std::uint32_t ms, int what_ = RECV);			
            socket &keepalive (bool);
            socket &dontthrow (bool);

        protected:			
            socket &__set_option (int level, int option, void const *data, int length);
            socket &__get_option (int level, int option, void *data, int *length);
            std::intptr_t __is_closed (std::intptr_t in_);

            socket (std::intptr_t desc_, ax::address const &addr_);

        private:
            ax::address addr_;
            std::intptr_t desc_;
            bool nothrow_;
        };

    }
}

#endif
